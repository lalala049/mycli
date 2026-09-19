#include "cli.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Operation{
    char* cmd;
    CliArgs args;
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

void cli_register_command(const char* cmd_name,const char* help,CliCommandHandler handler){
    Operation* opt = &opts[cmdCount];

    opt->cmd = str_clone(cmd_name);
    opt->help = str_clone(help);
    opt->handler = handler;
}

int cli_run(int argc, char **argv)
{
    int i=1;
    if(strcmp(argv[1],opts[0].cmd) == 0){
        CliArgs args;
        args.operated = argv[2];
        i = opts[0].handler(args);
    }

    printf("你运行了%s，运行是否成功：%i\n",opts[0].cmd,i);
    return 0;
}
