#include"cli.h"
#include<stdio.h>
#include<string.h>

int cmd_cnm(int argc,CliArg* args){
    //
    // for(int i = 0;i < argc;i++){
    //     CliArg* arg = &args[i];
    //
    //     switch(arg -> type)
    //     {
    //         case CLI_FLAG:
    //
    //         break;
    //
    //         case CLI_POS:
    //
    //         break;
    //     }
    // }
    for(int i = 0;i < argc;i++){
        char* something = args[i].type == CLI_FLAG ? args[i].flag : args[i].pos;
        printf("看看把你输入的参数%s\n",something);
    }

    return 0;
}

int main(int argc,char **agrv){

    cli_register_command("cnm","这也要help",cmd_cnm);

    int failure = cli_run(argc,agrv);
    if(failure){
        printf("好像运行失败了");
    }

    return 0;
}
