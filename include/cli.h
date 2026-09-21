#ifndef CLI_H
#define CLI_H

typedef enum{
    CLI_FLAG,
    CLI_POS,
}CliArgType;

typedef struct CliArg{
    CliArgType type;
    char* flag;
    char* pos;
}CliArg;

typedef int (*CliCommandHandler)(int argc,CliArg* args);

int cli_run(int argc,char** argv);

void cli_register_command(const char* cmd_name,const char* help,CliCommandHandler handler);

void cli_command_help(const char* cmd_name);

#endif
