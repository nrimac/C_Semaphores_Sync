#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#define BUD 4
#define BRD 6
#define BID 3
#define VEL_MS 7

sem_t writeSem;

void stvoriUlazneDretve();
void stvoriRadneDretve();
void stvoriIzlazneDretve();

int dohvatiUlaz();
int obradiUlaz();

void fillMS();
void printUMS();
void printIMS();

void init() {
    sem_init(&writeSem, 0, 1);
}

int main() {
    init();

    stvoriUlazneDretve();

    return 0;
}

void fillMS(char* UMS, char* IMS) {
    for (int i = 0; i < BRD; i++) {
        for (int j = 0; j < VEL_MS; j++) {
            *(UMS + i * VEL_MS + j) = '-';
        }
    }

    for (int i = 0; i < BID; i++) {
        for (int j = 0; j < VEL_MS; j++) {
            *(IMS + i * VEL_MS + j) = '-';
        }
    }
}

void printUMS(char* UMS) {
    printf("UMS[]: ");
    for (int i = 0; i < BRD; i++) {
        for (int j = 0; j < VEL_MS; j++) printf("%c", *(UMS + i * VEL_MS + j));
        printf(" ");
    }
    printf("\n");
}

void printIMS(char* IMS) {
    printf("IMS[]: ");
    for (int i = 0; i < BID; i++) {
        for (int j = 0; j < VEL_MS; j++) printf("%c", *(IMS + i * VEL_MS + j));
        printf(" ");
    }
    printf("\n");
}

int dohvatiUlaz(int id) {
    srand(time(NULL) + id);
    return 'A' + rand() % 26;
}

int obradiUlaz(int id, int ulaz) {
    return (ulaz + id) % BRD;
}

void *ulaz(void * threadId) {
    char UMS[BRD][VEL_MS];
    char IMS[BID][VEL_MS];
    const int id = (int) threadId;

    fillMS(UMS, IMS);
    stvoriRadneDretve();

    while (1) {
        const int U = dohvatiUlaz(id);
        const int T = obradiUlaz(id, U);

        //K.O.
        sem_wait(&writeSem);
        UMS[T][0] = U;
        printf("U%d: dohvatiUlaz(%d) => '%c', obradiUlaz(%c) => %d\n", id, id, U, U, T);
        printUMS(UMS);
        printIMS(IMS);
        printf("\n");
        sem_post(&writeSem);
        //Kraj K.O.

        sleep(5);
    }
}

void *rad(void * threadId) {
    int id = (int) threadId;
    //printf("id: %d\n", id);
}

void *izlaz(void * threadId) {

}

void stvoriUlazneDretve() {
    pthread_t threads[BUD];

    for (int i = 0; i < BUD; i++) {
        pthread_t ulaznaDretva;
        pthread_create(&ulaznaDretva, NULL, ulaz, i);
        threads[i] = ulaznaDretva;
    }

    for (int i = 0; i < BUD; i++) {
        pthread_join(threads[i], NULL);
    }
}

void stvoriRadneDretve() {
    pthread_t threads[BRD];

    for (int i = 0; i < BRD; i++) {
        pthread_t radnaDretva;
        pthread_create(&radnaDretva, NULL, rad, i);
        threads[i] = radnaDretva;
    }

    for (int i = 0; i < BRD; i++) {
        pthread_join(threads[i], NULL);
    }
}

void stvoriIzlazneDretve() {
    pthread_t threads[BID];

    for (int i = 0; i < BID; i++) {
        pthread_t izlaznaDretva;
        pthread_create(&izlaznaDretva, NULL, izlaz, i);
        threads[i] = izlaznaDretva;
    }

    for (int i = 0; i < BID; i++) {
        pthread_join(threads[i], NULL);
    }
}