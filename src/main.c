#include"cli.h"
#include<stdio.h>

int cmd_cnm(CliArgs args){
    printf("这都能运行起来%s\n",args.operated);
    return 0;
}

int main(int argc,char **agrv){

    cli_register_command("cnm","这也要help",cmd_cnm);

    cli_run(argc,agrv);

    return 0;
}
