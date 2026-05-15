#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    int pid;
    volatile int i;

    pid = fork();

    if(pid < 0){
        printf("Fork failed\n");
        exit(1);
    }

    if(pid == 0){

        // Child process
        while(1){
            printf("Child running...\n");

            for(i = 0; i < 100000000; i++);
        }

    } else {

        // Parent process
        for(i = 0; i < 100000000; i++);

        printf("Freezing child process %d\n", pid);

        freeze(pid);

        for(i = 0; i < 100000000; i++);

        printf("Resuming child process %d\n", pid);

        resume(pid);

        while(1){
            for(i = 0; i < 100000000; i++);
        }
    }

    exit(0);
}
