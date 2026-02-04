/*
Error koder
0 Ok
-1 Feil med fork
-2 Kunne ikke opprette pipe
-3 Kunne ikke skirve til pipe
-4 Kunne lese fra pipe
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int main() {
    int pid=0;
    int fd[2];
    int ret_pipe=0;
    char str[100];
    ret_pipe = pipe(fd);
    pid = fork();

    if (ret_pipe != 0) {
        return -2;
    }

    if (pid == -1) {
        printf("Kunne ikke forke\n");
        return -1;
    }

    if (pid == 0) { //child
       close(fd[1]); 
       char lest[100];
       int antall=0;
       antall = read(fd[0], lest, sizeof(lest));
       printf("Lest er %s\n", lest);
       close(fd[0]);
       if (antall == -1) {
            printf("Kunne ikke lese fra pipe\n");
            return -4;
       }
    }
    else {  //forelder
        close(fd[0]);
//        str="abc"; dette er feil
        strcpy(str, "abc"); // dette er riktig
        int antall=0;
        antall = write(fd[1], str, sizeof(str)); 
        if (antall == -1) {
            printf("Kunne ikke skrive til pipe\n");
            return -3;
        }
        close(fd[1]);
        wait(NULL);
    }
    return 0;
}
