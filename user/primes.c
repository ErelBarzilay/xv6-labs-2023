#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include <stddef.h>

 int recursive_primes_function(int* curr_suspected_primes, int len){
    if (len == 0){
        return 0;
    }
    int dad_write[2];
    if (pipe(dad_write)){
        fprintf(2,"error piping");
        return -1;
    }
    int pid = fork();
    if (pid == -1){
        return -1;
    }
    if (pid == 0){ //we are at the son: we wish to transfer our data to the function in the next call
        close(dad_write[1]);
        int* arr = (int*) malloc(len * sizeof(int));
        if (arr == NULL){
            fprintf(2,"seg fault");
            close(dad_write[0]);
            exit(-1);
        }
        int curr_len = 0;
        int addition;
        while ((addition = read(dad_write[0],arr+curr_len,len * 4)) > 0){
            curr_len = curr_len + addition/4;
        }
        close(dad_write[0]);
        recursive_primes_function(arr, curr_len);
        free(arr);
        exit(0);        
    }
    close(dad_write[0]);
    int new_suspected_primes[len - 1];
    int curr_len = 0;
    int curr;
    int divider = curr_suspected_primes[0];
    fprintf(1, "prime %d\n", divider);
    //fprintf(1, "%d =len\n", len);
    for (int i = 1; i < len; i++){
        curr = curr_suspected_primes[i];
        if (curr%divider != 0){
            //fprintf(1,"%d\n",curr);
            new_suspected_primes[curr_len] = curr;
            curr_len ++;
        }
    }
    write(dad_write[1], new_suspected_primes, curr_len * sizeof(int));
    close(dad_write[1]);
    wait(NULL);
    return 0;
}
int main(int argc, char *argv[]){
    int len = 34;
    int nums[len];
    nums[0] = 0;
    for (int i =0; i < len; i++){
        nums[i] = i + 2;
    }
    int k = (recursive_primes_function(nums,len));
    exit(k);
}