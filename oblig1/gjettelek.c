/* 
 * Feilkoder:
 * 1 - Fork feilet
 * 2 - Pipe opprettelse feilet (parent to child)
 * 3 - Pipe opprettelse feilet (child to parent)
 * 4 - Lesing fra pipe feilet
 * 5 - Skriving til pipe feilet
 * 6 - Venting på barneprosess feilet
 * 7 - Barneprosess avsluttet med feil 
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>

#define MIN_VERDI 0
#define MAX_VERDI 1000
#define FOR_HØY 1
#define RIKTIG 0
#define FOR_LAV -1

int main(void) {
    int pipeForelderTilBarn[2];
    int pipeBarnTilForelder[2];
    pid_t pid;

    // Opprett pipe fra forelder til barn
    if (pipe(pipeForelderTilBarn) == -1) {
        perror("Feil ved opprettelse av pipe (forelder->barn)");
        exit(2);
    }

    // Opprett pipe fra barn til forelder
    if (pipe(pipeBarnTilForelder) == -1) {
        perror("Feil ved opprettelse av pipe (barn->forelder)\n");
        close(pipeForelderTilBarn[0]);
        close(pipeBarnTilForelder[1]);
        exit(3);
    }

    // Opprett barneprosess
    pid = fork();

    if (pid == -1) {
        perror("Feil ved fork()\n");
        close(pipeForelderTilBarn[0]);
        close(pipeForelderTilBarn[1]);
        close(pipeBarnTilForelder[0]);
        close(pipeBarnTilForelder[1]);
        exit(1);
    }

    if (pid == 0) {
        // Barneprosess

        // Lukk ubrukte pipe ender
        close(pipeForelderTilBarn[1]);
        close(pipeBarnTilForelder[0]);

        srand(time(NULL) ^ getpid());
        int hemmelig = rand() % (MAX_VERDI + 1);

        int gjett = 0;
        int svar = 0;
        ssize_t bytesLest = 0;
        ssize_t bytesSkrevet = 0;

        while (1) {
            bytesLest = read(pipeForelderTilBarn[0], &gjett, sizeof(int));
            if (bytesLest == -1) {
                perror("Barneprosess: Feil ved lesing fra pipe\n");
                close(pipeForelderTilBarn[0]);
                close(pipeBarnTilForelder[1]);
                exit(4);
            }

            if (gjett > hemmelig) {
                svar = FOR_HØY;
            } else if (gjett < hemmelig) {
                svar = FOR_LAV;
            } else {
                svar = RIKTIG;
            }
            
            bytesSkrevet = write(pipeBarnTilForelder[1], &svar, sizeof(int));
            if (bytesSkrevet == -1) {
                perror("Barneprosess: Feil ved skriving til pipe\n");
                close(pipeForelderTilBarn[0]);
                close(pipeBarnTilForelder[1]);
                exit(5);
            }

            if(svar == RIKTIG) {
                break;
            }
        }

        close(pipeForelderTilBarn[0]);
        close(pipeBarnTilForelder[1]);
        exit(0);

    } else {
        // Foreldrerposess
        
        close(pipeForelderTilBarn[0]);
        close(pipeBarnTilForelder[1]);

        int nedreGrense = MIN_VERDI;
        int øvreGrense = MAX_VERDI;
        int gjett = 0;
        int svar= 0;
        int antallForsøk = 0;
        ssize_t bytesSkrevet = 0;
        ssize_t bytesLest = 0;

        while(1) {

            gjett = nedreGrense + (øvreGrense - nedreGrense) / 2;
            antallForsøk++;

            bytesSkrevet = write(pipeForelderTilBarn[1], &gjett, sizeof(int));
            if (bytesSkrevet == -1) {
                perror("Foreldreprosess: Feil ved skriving til pipe\n");
                close(pipeForelderTilBarn[1]);
                close(pipeBarnTilForelder[0]);
                exit(5);
            }

            bytesLest = read(pipeBarnTilForelder[0], &svar, sizeof(int));
            if (bytesLest == -1) {
                perror("Foreldreprosess: Feil ved lesing fra pipe\n");
                close(pipeForelderTilBarn[1]);
                close(pipeBarnTilForelder[0]);
                exit(4);
            }

            if (svar == RIKTIG) {
                break;
            } else if (svar == FOR_HØY) {
                øvreGrense = gjett - 1;
            } else if (svar == FOR_LAV) {
                nedreGrense = gjett + 1; 
            }
        }

        close(pipeForelderTilBarn[1]);
        close(pipeBarnTilForelder[0]);

        int status = 0;
        if (wait(&status) == -1) {
            perror("Feil ved venting på barneprosess\n");
            exit(6);
        }

        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) != 0) {
                fprintf(stderr, "Barneprosess avsluttet med feilkode %d\n", WEXITSTATUS(status));
                exit(7);
            }
        }
        
        printf("Foreldreprosessen gjettet tallet %d på %d forsøk.\n", gjett, antallForsøk);
    }

    return 0;
}
