#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int
main(int argc, char *argv[])
{
    int son_writing[2];
    int dad_writing[2];
    if ((pipe(son_writing) == -1) || (pipe(dad_writing) == -1)){
        exit(-1);
    }
    int pid;
    pid = fork();
    if (pid == -1){
        exit(-1);
    }

    if (pid == 0){
        char a[1];
        a[0] = 'b';
        while (!read(dad_writing[0],a,1));
        close(dad_writing[0]);
        pid = getpid();
        fprintf(1,"%d: received ping\n", pid);
        close(son_writing[0]);

        write(son_writing[1],a,1);
        close(son_writing[1]);
        close(dad_writing[1]);

        exit(0);
    }
    else{
        pid = getpid();
        close(dad_writing[0]);
        char a[1];
        a[0] = 'b';
        write(dad_writing[1],a,1);
        close(dad_writing[1]);
        close(son_writing[1]);
        while (!read(son_writing[0],a,1)){
            ;
        }
        close(son_writing[0]);
        fprintf(1,"%d: received pong\n", pid);
    }
    exit(0);
}
