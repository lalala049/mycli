#include"cli.h"
#include<stdio.h>
#include<string.h>

int cmd_todo(int argc,char** args){
    if(cli_flag_exist("help")){
        cli_command_help("todo");
    }
    return 0;
}

int main(int argc,char **agrv){

    cli_register_command("todo","这里是todo的帮助文档，需要帮助吗",cmd_todo);

    int failure = cli_run(argc,agrv);
    if(failure){
        printf("你的命令运行失败了");
    }

    return 0;
}
