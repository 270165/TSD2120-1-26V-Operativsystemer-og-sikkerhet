#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define TH 2

void *thread_function(void *arg) {
    char *name = malloc(100 * sizeof(char));
    printf("Skriv inn navnet ditt:\n");
    scanf("%s", name);
    int thread_nr = *(int*)arg;
    snprintf(name, 100, "%d Mitt navn er Geir\n", thread_nr);

    for (int i = 0; i < TH; i++) {
        printf("Thread_function for threadnr. %d i=%d\n", thread_nr, i);
        sleep(1);
    }
    return (void*)name;
}

void *thread_function2(void *arg) {
    int thread_nr = *(int*)arg;
    int *retur = malloc(sizeof(int));
    *retur = 10 * thread_nr;
    
    for ( int i = 0; i < TH; i++) {
        printf("Thread_function for threadnr. %d i=%d\n", thread_nr, i);
        sleep(1);
    }
    return (void*)retur;
}

int main(void) {
    pthread_t threads[TH];
    int thread_ids[TH];
    char *str;

    for (int i = 0; i < TH; i++) {
        thread_ids[i] = i + 1;
        if (i % 2 == 0) {
            if (pthread_create(&threads[i], NULL, thread_function, (void*)&thread_ids[i]) !=0) {
                perror("Feil ved opprettelse av tråd");
                exit(1);
            }
        } else { 
            if (pthread_create(&threads[i], NULL, thread_function, (void*)&thread_ids[i]) != 0) {
                perror("Feil ved opprettelse av tråd");
                exit(1);
            }
        }
    }

    for (int i = 0; i < TH; i++) {
        pthread_join(threads[i], (void**)&str);
        printf("Return from thread %d : 0x%X\n", i, &str);
        free(str);
    }

    printf("Alle trådene er ferdig.\n");
    return 0;
}
