#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

#define MAX_LENGTH 1024

void
execute(char* comm, char** params) //just executes the command based on the params
{
    int pid = fork();
    if (pid > 0) {
        wait(&pid);
    } 
    else {
    exec(comm, params);
    }
}

void
xargs(char** first_arg, int size, char* command)
{
    char buf[MAX_LENGTH];
    char *arg[MAXARG];
    int m = 0;
    while (read(0, buf+m, 1) == 1) {
        if (m >= MAXARG - 1) {
            fprintf(2, "xargs: arguments too long.\n");
            exit(1);
        }
        if (buf[m] == '\n')   //end of line, meaning we have to run now the code for the line
        {
            buf[m] = '\0';
            memmove(arg, first_arg, sizeof(*first_arg)*size); //move first arg, i.e. tje known args, to the beginning of arg
            int arg_index = size;
            if (arg_index == 0) {
                arg[arg_index] = command;
                arg_index++;
            }
            arg[arg_index] = malloc(sizeof(char)*(m+1));
            memmove(arg[arg_index], buf, m+1); //now move the data in the buffer to arg[arg_index]
            arg[arg_index+1] = 0;
            execute(command, arg); //now execute the command
            free(arg[arg_index]); 
            m = 0;
        } else 
        {
            m++;
        }
    }
}


int
main(int argc, char* argv[])
{
    char* comm = argv[1];
    xargs(argv + 1, argc - 1, comm);
    exit(0);
}
