#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
    int n = 0, m = 0;
    for (int i = 0; i < atoi(argv[1]); i++) {
        printf("%c%c", 'A'+n, 'A'+m);
        if(m == 25) {
            m = m%26;
            n++;
            m = 1;
        }
        m++;
    }
    printf("\n");
    
    return 0;
 }
