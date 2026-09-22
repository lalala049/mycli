#include "cli.h"

#include <stdio.h>

/* todo 命令的 handler。
 * 注意：收到的是"命令名之后"的纯参数（不含命令名本身），
 * 开关类参数（-xxx / --xxx）不会出现在 args 里，要用 cli_flag_exist 查询。 */
static int cmd_todo(int argc, char **args) {
    (void)argc; /* 本示例不用参数，显式声明"我故意不用"，消除 -Wunused-parameter 警告 */
    (void)args;

    if (cli_flag_exist("help")) {
        cli_command_help("todo");
        return 0;
    }

    printf("你没有传任何开关。试试: myCLI todo --help\n");
    return 0;
}

/* 演示用 echo 命令：把普通参数原样打印出来。
 * 用来直观验证参数解析是否把命令名错传给了 handler。 */
static int cmd_echo(int argc, char **args) {
    for (int i = 0; i < argc; i++) {
        printf("[%d] %s\n", i, args[i]);
    }
    return 0;
}

int main(int argc, char **argv) { /* 原代码把 argv 拼写成 agrv，已修正 */
    if (cli_register_command("todo", "这里是todo的帮助文档，需要帮助吗", cmd_todo) != 0 ||
        cli_register_command("echo", "把参数原样打印出来", cmd_echo) != 0) {
        fprintf(stderr, "命令注册失败\n");
        return -1;
    }

    int failure = cli_run(argc, argv);
    if (failure) {
        printf("命令执行失败，退出码: %d\n", failure);
    }

    /* 原代码无论成败都 return 0，脚本/CI 无法判断结果。
     * 现在把框架或 handler 的失败码透传给进程退出码。 */
    return failure;
}
