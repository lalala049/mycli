#include"cli.h"
#include<stdio.h>
#include<string.h>

int cmd_cnm(CliArgs args){
    if(strcmp(args.boolflag[0],"help") == 0){
        cli_command_help("cnm");
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
