#include "cli.h"
#include "map.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Operation{
    char* cmd;
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

static int min(int a,int b){
    return a > b ? b : a;
}

static int levenshtein(const char* s1,const char* s2){

    int n = strlen(s1),m = strlen(s2);
    int dp[n+1][m+1];
    for(int i = 0;i <= n;i++) dp[i][0] = i;
    for(int j = 0;j <= m;j++) dp[0][j] = j;

    for(int i = 1;i <= n;i++){
        for(int j = 1;j <= m;j++){
            if(s1[i-1] == s2[j-1]){
                dp[i][j] = dp[i-1][j-1];
            }else{
                dp[i][j] = 1 + min(dp[i-1][j],min(dp[i-1][j-1],dp[i][j-1]));
            }
        }
    }

    return dp[n][m];
}

static void wrong_command(const char* cmd_wrong){
    int min = 10000;
    int res = -1;

    for(int i = 0;i < cmdCount;i++){
        int chenge = levenshtein(cmd_wrong,opts[i].cmd);
        if(chenge < min){
            min = chenge;
            res = i;
        }
    }
    printf("错误的命令:%s\n你想输入的命令可能是:",cmd_wrong);
    if(res != -1) printf("%s\n",opts[res].cmd);
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

static char** parse_argv_to_cliargs(int* argc,char** argv){
    int argc_int = (*argc) - 1;
    char** args = malloc(*argc*sizeof(char*));
    int args_count = 0;

    for(int i = 0;i < argc_int; i++){
        if(strlen(argv[i]) >= 2 && argv[i][0] == '-'){
            map_put(argv[i]+1,1);
        }else{
            args[args_count] = argv[i];
            args_count++;
        }
    }

    *argc = args_count;

    return args;
}

int cli_flag_exist(char* flag){
    return map_get(flag);
}

void cli_command_help(const char* cmd_name){
    int cmd_index = find_cmd(cmd_name);
    if(cmd_index == -1){
        return;
    }

    printf("%s的帮助文档\n%s\n",cmd_name,opts[cmd_index].help);
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
    if(argc <= 0){
        return -1;
    }
    int cmd_index = find_cmd(argv[1]);
    if(cmd_index == -1){
        return -1;
    }

    char** args = parse_argv_to_cliargs(&argc,argv+1);
    opts[cmd_index].handler(argc,args);

    return 0;
}
