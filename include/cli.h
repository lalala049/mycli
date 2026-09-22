#ifndef CLI_H
#define CLI_H

typedef int (*CliCommandHandler)(int argc,char** args);

int cli_run(int argc,char** argv);

void cli_register_command(const char* cmd_name,const char* help,CliCommandHandler handler);

void cli_command_help(const char* cmd_name);

int cli_flag_exist(char* flag);

#endif
