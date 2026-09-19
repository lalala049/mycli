#ifndef CLI_H
#define CLI_H

typedef struct CliArgs{
    char* boolflag;
    char* operated;
}CliArgs;

typedef int (*CliCommandHandler)(CliArgs args);

int cli_run(int argc,char** argv);

void cli_register_command(const char* cmd_name,const char* help,CliCommandHandler handler);

#endif
