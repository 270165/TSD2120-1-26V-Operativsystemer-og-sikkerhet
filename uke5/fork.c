#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>


int main() {
    int pid = 0;
    int pid2 = 0;
    int status1 = 0;
    int status2 = 0;

    pid = fork();

    if(pid == -1) {
        printf("Kunne ikke forke\n");
        return -1;
    }

    if(pid == 0) {
        pid2 = fork();
        
        if(pid2 == -1) {
            printf("Kunne ikke forke\n");
            return -1;
        }
        if(pid2 > 0) {
            printf("Dette er child-parent\n");
            waitpid(pid2, &status2, 0);
        }
        else {
            printf("Dette er child-child\n"); 
        }
        
        exit(0);
    }

    else {
        pid2 = fork();

        if(pid2 == -1) {
            printf("Kunne ikke forke\n");
            return -1;
        }
        if(pid2 > 0) {
            printf("Dette er parent-child\n");
            waitpid(pid, &status1, 0);
            waitpid(pid2, &status2, 0);
        }
        else {
            printf("Dette er parent-child\n");
            exit(0);
        }
    }
    
    return 0;
}



