#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
/*    int tall = rand()%1000;
    int gjettning = 0;

    printf("Skriv inn  tallet som du tror jeg har tenker på:\n");
    scanf("%d", &gjettning);

    if(tall == gjettning) {
        printf("Du hadde riktig\n");
    }
    else printf("Du hadde feil\n");
 */   
    
    char buffer[10];
    strcpy(buffer, argv[1]);
    printf("Buffer er %s\n", buffer);

    return 0;
}
