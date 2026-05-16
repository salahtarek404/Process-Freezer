#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[]){
    int pid;
    volatile int i;
    pid = fork();

    if(pid == 0){
        while(1){
            printf("Child running...\n");
            for(i = 0; i < 100000000; i++);
        }
    } else {
        for(i = 0; i < 100000000; i++);
        printf("Freezing child process %d\n", pid);
        freeze(pid);
        while(1);
    }

    exit(0);
}
