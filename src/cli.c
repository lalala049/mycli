#include "cli.h"
#include "map.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COMMANDS 36         /* 命令数上限（代替魔法数字 36） */
#define SUGGEST_MAX_DIST 2      /* 拼写建议的最大编辑距离，防止乱输入也硬给建议 */
#define LEVENSHTEIN_MAX_LEN 128 /* 编辑距离输入长度上限，保护 VLA 不爆栈 */

typedef struct Operation {
    char *cmd;
    CliCommandHandler handler;
    char *help;
} Operation;

static Operation opts[MAX_COMMANDS];
static int cmdCount = 0;

/* 复制字符串到堆上（strdup 是 POSIX 函数，C11 标准里没有，所以手写）。 */
static char *str_clone(const char *src) {
    if (src == NULL) return NULL;
    size_t len = strlen(src);
    char *buf = malloc(len + 1);
    if (buf == NULL) return NULL;
    memcpy(buf, src, len);
    buf[len] = '\0';
    return buf;
}

/* 原代码把局部变量命名为 min，遮蔽了同名的 min() 函数，可读性差；改名避免混淆。 */
static int min_int(int a, int b) {
    return a < b ? a : b;
}

/* Levenshtein 编辑距离（DP 实现）。
 * 防御：输入超长时直接返回一个"足够远"的距离，避免 VLA 在栈上爆掉。
 * dp 是边长可变的大数组，栈上分配；用户输入不受控时越界风险真实存在。 */
static int levenshtein(const char *s1, const char *s2) {
    if (s1 == NULL || s2 == NULL) return LEVENSHTEIN_MAX_LEN;

    int n = (int)strlen(s1);
    int m = (int)strlen(s2);
    if (n > LEVENSHTEIN_MAX_LEN || m > LEVENSHTEIN_MAX_LEN) {
        return LEVENSHTEIN_MAX_LEN;
    }

    int dp[n + 1][m + 1];
    for (int i = 0; i <= n; i++) dp[i][0] = i;
    for (int j = 0; j <= m; j++) dp[0][j] = j;

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            if (s1[i - 1] == s2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            } else {
                dp[i][j] = 1 + min_int(dp[i - 1][j], min_int(dp[i - 1][j - 1], dp[i][j - 1]));
            }
        }
    }
    return dp[n][m];
}

/* 给出"你是不是想输这个"的建议。
 * 原来没有距离阈值：随便敲什么都会硬挑一个"最接近"的命令，反而误导。
 * 现在只在编辑距离 <= SUGGEST_MAX_DIST 时才点名建议，否则列出全部命令。 */
static void wrong_command(const char *cmd_wrong) {
    if (cmd_wrong == NULL) {
        printf("未知命令\n");
        return;
    }

    int best_dist = SUGGEST_MAX_DIST + 1; /* 初始值已超阈值，表示"没有合格的候选" */
    int best_index = -1;

    for (int i = 0; i < cmdCount; i++) {
        int dist = levenshtein(cmd_wrong, opts[i].cmd);
        if (dist < best_dist) {
            best_dist = dist;
            best_index = i;
        }
    }

    printf("未知命令: %s\n", cmd_wrong);
    if (best_index != -1) {
        printf("你是不是想输入: %s\n", opts[best_index].cmd);
    } else {
        printf("没有相近的命令，可用命令如下:\n");
        cli_print_help_all();
    }
    printf("\n");
}

static int find_cmd(const char *cmd_name) {
    if (cmd_name == NULL) return -1;

    for (int i = 0; i < cmdCount; i++) {
        if (strcmp(opts[i].cmd, cmd_name) == 0) {
            return i;
        }
    }

    wrong_command(cmd_name);
    return -1;
}

/* 从"命令名之后"的纯参数列表里挑出普通参数，把 "-xxx" 形式的开关登记进 map。
 *
 * 原代码有两个问题：
 *   1. cli_run 传的是 argv+1（第一个元素是命令名），parse 却从 0 开始遍历，
 *      结果 handler 收到的 args[0] 是命令名本身 —— 参数错位。
 *   2. 一次性 malloc(argc) 个指针，实际可能只用一小半，浪费且每次调用泄漏一次。
 *
 * 现在：只处理纯参数（不含命令名），两遍法精确分配；
 * 返回的数组由 cli_run 统一 free。 */
static char **parse_args(int *argc, char **argv) {
    int total = *argc;

    /* 第一遍：数出有多少个普通参数，按需分配 */
    int n = 0;
    for (int i = 0; i < total; i++) {
        if (!(strlen(argv[i]) >= 2 && argv[i][0] == '-')) {
            n++;
        }
    }

    char **args = malloc((n > 0 ? (size_t)n : 1) * sizeof(char *));
    if (args == NULL) return NULL;

    /* 第二遍：填表，同时把开关登记进哈希表 */
    int count = 0;
    for (int i = 0; i < total; i++) {
        if (strlen(argv[i]) >= 2 && argv[i][0] == '-') {
            /* 原代码只跳过了一个 '-'：--help 注册成 key="-help"，
             * handler 查 "help" 永远查不到，帮助开关从未生效。
             * 正确做法是跳过全部前导 '-'："--help" -> "help"，"-h" -> "h"。 */
            const char *flag = argv[i];
            while (*flag == '-') flag++;
            map_put(flag, 1);
        } else {
            args[count++] = argv[i];
        }
    }

    *argc = count;
    return args;
}

int cli_flag_exist(const char *flag) {
    return map_get(flag);
}

void cli_command_help(const char *cmd_name) {
    int cmd_index = find_cmd(cmd_name);
    if (cmd_index == -1) return; /* find_cmd 内部已打印建议，这里静默返回 */

    printf("命令 %s 的帮助:\n%s\n\n", cmd_name, opts[cmd_index].help);
}

void cli_print_help_all(void) {
    if (cmdCount == 0) {
        printf("  (还没有注册任何命令)\n");
        return;
    }
    for (int i = 0; i < cmdCount; i++) {
        printf("  %-12s %s\n", opts[i].cmd, opts[i].help);
    }
}

int cli_register_command(const char *cmd_name, const char *help, CliCommandHandler handler) {
    if (cmd_name == NULL || help == NULL || handler == NULL) return -1;

    /* 原来的代码直接写 opts[cmdCount]，超过 36 条命令就是缓冲区越界 ——
     * C 语言里最危险的未定义行为之一。现在显式检查并报错。 */
    if (cmdCount >= MAX_COMMANDS) {
        fprintf(stderr, "cli: 命令数已达上限 %d，无法注册 \"%s\"\n", MAX_COMMANDS, cmd_name);
        return -1;
    }

    Operation *opt = &opts[cmdCount];
    opt->cmd = str_clone(cmd_name);
    opt->help = str_clone(help);
    opt->handler = handler;
    cmdCount++;
    return 0;
}

int cli_run(int argc, char **argv) {
    /* 原来的检查是 argc <= 0，但 argc == 1（没带命令）时 argv[1] 是 NULL，
     * find_cmd -> strcmp(NULL) 直接访问违规，程序当场崩溃（实测 0xC0000005）。
     * 正确边界是 argc >= 2。 */
    if (argc < 2) {
        printf("用法: %s <命令> [参数...]\n\n可用命令:\n", argc > 0 ? argv[0] : "myCLI");
        cli_print_help_all();
        return -1;
    }

    /* 每次运行前清空上一次残留的 flag，避免跨调用状态污染 */
    map_clear();

    int cmd_index = find_cmd(argv[1]);
    if (cmd_index == -1) return -1;

    /* 只把"命令名之后"的参数交给 handler，命令名本身不进 args */
    int args_argc = argc - 2;
    char **args = parse_args(&args_argc, argv + 2);
    if (args == NULL) {
        fprintf(stderr, "cli: 参数解析内存分配失败\n");
        return -1;
    }

    int rc = opts[cmd_index].handler(args_argc, args);

    free(args); /* args 由 cli_run 统一释放（生命周期约定见 cli.h） */
    return rc;
}
