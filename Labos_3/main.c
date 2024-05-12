#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#define BUD 1
#define BRD 2
#define BID 3
#define VEL_MS 5

char UMS[BRD][VEL_MS];
char IMS[BID][VEL_MS];

pthread_t threads[BUD + BRD + BID];
sem_t writeSem;
sem_t readUMSSem[BRD];

//initialize threads
void stvoriUlazneDretve();
void stvoriRadneDretve();
void stvoriIzlazneDretve();

//thread functions
int dohvatiUlaz();
int obradiUlaz();
void obradi(int id, int podatak, int* r, int* t);

//helper functions
void fillMS();
void printUMS();
void printIMS();

void init() {
    fillMS();
    sem_init(&writeSem, 0, 1);

    for (int i = 0; i < BRD; i++) {
        sem_init(&readUMSSem[i], 0, 0);
    }
}

int main() {
    init();

    stvoriUlazneDretve();
    stvoriRadneDretve();
    stvoriIzlazneDretve();

    for (int i = 0; i < BUD + BRD + BID; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

void fillMS() {
    for (int i = 0; i < BRD; i++) {
        for (int j = 0; j < VEL_MS; j++) {
            UMS[i][j] = '-';
        }
    }

    for (int i = 0; i < BID; i++) {
        for (int j = 0; j < VEL_MS; j++) {
            IMS[i][j] = '-';
        }
    }
}

void printUMS() {
    printf("UMS[]: ");
    for (int i = 0; i < BRD; i++) {
        for (int j = 0; j < VEL_MS; j++) printf("%c", UMS[i][j]);
        printf(" ");
    }
    printf("\n");
}

void printIMS() {
    printf("IMS[]: ");
    for (int i = 0; i < BID; i++) {
        for (int j = 0; j < VEL_MS; j++) printf("%c", IMS[i][j]);
        printf(" ");
    }
    printf("\n");
}

int dohvatiUlaz(const int id) {
    srand(time(NULL) + id);
    return 'A' + rand() % 26;
}

int obradiUlaz(const int id, const int ulaz) {
    return (ulaz + id) % BRD;
}

void obradi(const int id, const int podatak, int* r, int* t) {
    *r = podatak + 32;
}

void *ulaz(void * threadId) {
    const int id = (int) threadId;

    int ulaz[BRD];

    while (1) {
        //K.O.
        sem_wait(&writeSem);
        const int U = dohvatiUlaz(id);
        const int T = obradiUlaz(id, U);

        UMS[T][ulaz[T]] = U;
        ulaz[T] = (ulaz[T] + 1) % VEL_MS;
        sem_post(&readUMSSem[T]);


        printf("U%d: dohvatiUlaz(%d) => '%c', obradiUlaz(%c) => %d\n", id, id, U, U, T);
        printUMS();
        printIMS();
        printf("\n");
        sem_post(&writeSem);
        //Kraj K.O.

        sleep(1);
    }
}

void *rad(void * threadId) {
    int izlaz[BRD];
    const int id = (int) threadId;

    while (1) {
        //K.O.
        sem_wait(&readUMSSem[id]);
        const int P = UMS[id][izlaz[id]];
        UMS[id][izlaz[id]] = "-";
        izlaz[id] = (izlaz[id] + 1) % VEL_MS;
        //obradi podatak
        int r, t;
        obradi(id, P, &r, &t);

        sem_wait(&writeSem);
        printf("R%d: UMS[%d][%d] => '%c'\n", id, id, izlaz[id], P);
        printUMS();
        printIMS();
        printf("\n");
        sem_post(&writeSem);
        //Kraj K.O.
    }
}

void *izlaz(void * threadId) {

}

void stvoriUlazneDretve() {
    for (int i = 0; i < BUD; i++) {
        pthread_t ulaznaDretva;
        pthread_create(&ulaznaDretva, NULL, ulaz, i);
        threads[i] = ulaznaDretva;
    }
}

void stvoriRadneDretve() {
    for (int i = 0; i < BRD; i++) {
        pthread_t radnaDretva;
        pthread_create(&radnaDretva, NULL, rad, i);
        threads[i + BUD] = radnaDretva;
    }
}

void stvoriIzlazneDretve() {
    for (int i = 0; i < BID; i++) {
        pthread_t izlaznaDretva;
        pthread_create(&izlaznaDretva, NULL, izlaz, i);
        threads[i + BRD + BUD] = izlaznaDretva;
    }
}