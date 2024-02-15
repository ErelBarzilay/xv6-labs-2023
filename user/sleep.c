#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc != 2){
    fprintf(1, "usage: sleep <int>");
    exit(0);
  }
  int sleep_amount = atoi(argv[1]);
  if (sleep(sleep_amount)!=0){
    fprintf(2, "error");
    exit(-1);
  }
  exit(0);
}