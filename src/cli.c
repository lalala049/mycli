#include "cli.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Operation{
    char* cmd;
    CliArg* args;
    CliCommandHandler handler;
    char* help;
}Operation;

static Operation opts[36];
static int cmdCount=0;

static char* str_clone(const char*src){
    if(src == NULL) return NULL;
    size_t len = strlen(src);
    char *buf = malloc(len + 1);
    if(!buf) return NULL;

    memcpy(buf,src,len);
    buf[len] = '\0';
    return buf;
}

static void wrong_command(const char* cmd_wrong){
    //TODO:我这里需要最小操作次数修改字符串算法，找出最适配的几个，做出提示
    printf("wrong command%s",cmd_wrong);
}

static int find_cmd(const char* cmd_name){
    for(int i = 0;i < cmdCount;i++){
        if(strcmp(opts[i].cmd,cmd_name) == 0){
            return i;
        }
    }

    wrong_command(cmd_name);
    return -1;
}

static CliArg* parse_argv_to_cliargs(int argc,char** argv){
    CliArg* args = malloc(sizeof(CliArg) * argc);
    if(!args) return NULL;

    for(int i = 0;i<argc;i++){
        if(strlen(argv[i]) >= 2 && argv[i][0] == '-'){
            args[i].type = CLI_FLAG;
            args[i].flag = argv[i] + 1;
        }else{
            args[i].pos = argv[i];
        }
    }

    return args;
}

void cli_command_help(const char* cmd_name){
    int cmd_index = find_cmd(cmd_name);
    if(cmd_index == -1){
        return;
    }

    printf("%s的帮助文档\n%s",cmd_name,opts[cmd_index].help);
}

void cli_register_command(const char* cmd_name,const char* help,CliCommandHandler handler){
    Operation* opt = &opts[cmdCount];
    cmdCount++;

    opt->cmd = str_clone(cmd_name);
    opt->help = str_clone(help);
    opt->handler = handler;
}

int cli_run(int argc, char **argv)
{
    //TODO:比较命令，选出执行的，取出参数，传递给处理器
    if(argc <= 0){
        return -1;
    }
    int cmd_index = find_cmd(argv[1]);
    if(cmd_index == -1){
        return -1;
    }

    CliArg* args = parse_argv_to_cliargs(argc-1,argv+1);
    opts[cmd_index].handler(argc-1,args);

    return 0;
}
