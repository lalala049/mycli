#ifndef CLI_H
#define CLI_H

/* myCLI 命令框架，使用流程：
 *   1. cli_register_command() 注册若干命令
 *   2. 调 cli_run(argc, argv) 解析命令行并分发给对应 handler
 *   3. handler 内可用 cli_flag_exist() 查询 "-xxx" 形式的开关
 *
 * 生命周期约定：cli_run 传给 handler 的 args 数组由 cli_run 负责释放，
 * handler 内不得长期持有（需要保存就自己拷贝）。
 */

typedef int (*CliCommandHandler)(int argc, char **args);

/* 注册一条命令。返回 0 表示成功；-1 表示参数非法或命令数已达上限。 */
int cli_register_command(const char *cmd_name, const char *help, CliCommandHandler handler);

/* 解析并执行命令行。返回 0 表示成功；非 0 表示失败
 * （负数：框架层失败，如命令不存在；正数：handler 返回的失败码）。 */
int cli_run(int argc, char **argv);

/* 打印某条命令的帮助。命令不存在时静默返回。 */
void cli_command_help(const char *cmd_name);

/* 打印全部已注册命令。命令缺失/输入为空时由框架自动调用。 */
void cli_print_help_all(void);

/* 查询开关是否存在（如传了 "--help" 则 flag="help" 存在）。 */
int cli_flag_exist(const char *flag);

#endif
