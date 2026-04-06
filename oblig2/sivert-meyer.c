/*
 * Feilkoder:
 * 1 - Feil ved åpning av fil
 * 2 - Feil ved opprettelse av tråd
 * 3 - Feil ved joining av tråd
 * 4 - Feil ved mutex-initialisering
 * 5 - Feil ved condition variable-initialisering
 * 6 - Feil ved låsing av mutex
 * 7 - Feil ved opplåsing av mutex
 * 8 - Feil ved lesing fra fil
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

// Global variabel
int balanse = 0;

pthread_mutex_t balanseMutex;
pthread_cond_t nokPengerCond;

// Funksjonsprototyper
void *innskuddTråd(void *arg);
void *uttakTråd(void *arg);


int main(void) {
    pthread_t innskuddTid;
    pthread_t uttakTid;
    int resultat = 0;

    // Initialiser mutex
    resultat = pthread_mutex_init(&balanseMutex, NULL);
    if (resultat != 0) {
        fprintf(stderr, "Feil ved initialisering av mutex: %d\n", resultat);
        exit(4);
    }

    // Initialiser condition variable
    resultat = pthread_cond_init(&nokPengerCond, NULL);
    if (resultat != 0) {
        fprintf(stderr, "Feil ved initialisering av condition variable: %d\n", resultat);
        pthread_mutex_destroy(&balanseMutex);
        exit(5);
    }

    // Opretter innskudsstråd
    resultat = pthread_create(&innskuddTid, NULL, innskuddTråd, NULL);
    if (resultat != 0) {
        fprintf(stderr, "Feil ved opprettelse av innskuddstråd: %d\n", resultat);
        pthread_mutex_destroy(&balanseMutex);
        pthread_cond_destroy(&nokPengerCond);
        exit(2);
    }

    // Opretter uttakstråd
    resultat = pthread_create(&uttakTid, NULL, uttakTråd, NULL);
    if (resultat != 0) {
        fprintf(stderr, "Feil ved joing av uttakstråd: %d\n", resultat);
        pthread_join(innskuddTid, NULL);
        pthread_mutex_destroy(&balanseMutex);
        pthread_cond_destroy(&nokPengerCond);
        exit(2);
    }

    resultat = pthread_join(uttakTid, NULL);
    if (resultat != 0) {
        fprintf(stderr, "Feil ved joing av uttakståd: %d\n", resultat);
        exit(3);
    }

    printf("Endelig balanse %d kr\n", balanse);
    
    pthread_mutex_destroy(&balanseMutex);
    pthread_cond_destroy(&nokPengerCond);

    return 0;
}

void *innskuddTråd(void *arg) {
    (void)arg;

    FILE *fil = NULL;
    int beløp = 0;
    int resultat = 0;

    fil = fopen("inn.txt", "r");
    if (fil == NULL) {
        perror("Feil ved åpning av inn.txt");
        pthread_exit((void *)1);
    }

    while (fscanf(fil, "%d", &beløp) == 1) {
        resultat = pthread_mutex_lock(&balanseMutex);
        if (resultat != 0) {
            fprintf(stderr, "Innskudsstråd: Feil ved låsing av mutex: %d\n", resultat);
            fclose(fil);
            pthread_exit((void *)6);
        }
        
        balanse += beløp;
        printf("Innskudd: +%d kr, ny balanse: %d kr\n", beløp, balanse);

        pthread_cond_broadcast(&nokPengerCond);

        resultat = pthread_mutex_unlock(&balanseMutex);
        if (resultat != 0) {
            fprintf(stderr, "Innskuddstråd: Feil ved opplåsing av mutex: %d\n", resultat);
            fclose(fil);
            pthread_exit((void *)7);
        }
    }

    if (ferror(fil)) {
        perror("Feil ved fra inn.txt");
        fclose(fil);
        pthread_exit((void *)8);
    }

    if (fclose(fil) != 0) {
        perror("Feil ved lukking av inn.txt");
    }

    pthread_cond_broadcast(&nokPengerCond);

    return NULL;
}

void *uttakTråd(void *arg) {
    (void)arg;

    FILE *fil = NULL;
    int beløp = 0;
    int resultat = 0;

    fil = fopen("ut.txt", "r");
    if (fil == NULL) {
        perror("Feil ved åpning av ut.txt");
        pthread_exit((void *)1);
    }

    while (fscanf(fil, "%d", &beløp) == 1) {
        resultat = pthread_mutex_lock(&balanseMutex);
        if (resultat != 0) {
            fprintf(stderr, "Uttakstråd: Feil ved låsing av mutex: %d\n", resultat);
            fclose(fil);
            pthread_exit((void *)6);
        }

        while (balanse < beløp) {
            printf("Uttak: Venter på %d kr, kun %d kr tilgjengelig\n", beløp, balanse);
            resultat = pthread_cond_wait(&nokPengerCond, &balanseMutex);
            pthread_mutex_unlock(&balanseMutex);
            if (resultat  != 0) {
                fprintf(stderr, "Uttakstråd: Feil ved venting på condition variable: %d\n", resultat);
                pthread_mutex_unlock(&balanseMutex);
                fclose(fil);
                pthread_exit((void *)6);
            }
        }

        balanse -= beløp;
        printf("Uttak: -%d kr, ny balanse: %d kr\n", beløp, balanse);

        resultat = pthread_mutex_unlock(&balanseMutex);
        if (resultat != 0) {
            fprintf(stderr, "Uttakstråd: Feil ved opplåsing av mute: %d\n", resultat);
            fclose(fil);
            pthread_exit((void *)7);
        }
    }
    
    if (ferror(fil)) {
        perror("Feil ved lesing fra ut.txt");
        fclose(fil);
        pthread_exit((void *)8);
    }

    if (fclose(fil) !=0) {
        perror("Feil ved lukking av ut.txt");
    }

    return NULL;
}
