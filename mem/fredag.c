#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define TH 2
int sum = 0;

pthread_mutex_t mutex;

void *thread_function(void *arg) {
    for (int tall = 0; tall < 100000; tall++) {    
        if (pthread_mutex_lock(&mutex) == 0) {
            sum++;
        } else {
            printf("Feil: kunne ikke låse Mutex\n");
            exit(5);
        }
        if (pthread_mutex_unlock(&mutex) != 0) {
            printf("Feil: kunne ikke låse opp Mutex\n");
            exit(6);
        }
    }
    return NULL;
}

int main(void) {
    pthread_t threads[TH];
    
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        printf("Feilet på mutex_init\n");
        exit(1);
    }

    for (int i = 0; i < TH; i++) {
        if (pthread_create(&threads[i], NULL, thread_function, NULL) != 0) {
            perror("Feil ved opprettelse av tråd\n");
            exit(2);
        }
    }

    for (int i = 0; i < TH; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            printf("Feilet på mutex_join\n");
            exit(3);
        }
    }

    printf("Summen er %d\n", sum);
    if (pthread_mutex_destroy(&mutex) != 0) {
        printf("Feilet på mutex_destroy\n");
        exit(4);
    }
    return 0;
}
