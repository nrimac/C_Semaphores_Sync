#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#define BUD 4
#define BRD 6
#define BID 3
#define VEL_MS 7

void stvoriUlazneDretve();
void stvoriRadneDretve();
void stvoriIzlazneDretve();

int dohvatiUlaz();
int obradiUlaz();

void fillMS();
void printUMS();
void printIMS();

int main() {
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

int obradiUlaz(int ulaz) {
    return (ulaz + getpid()) % BRD;
}

void *ulaz(void * threadId) {
    char UMS[BRD][VEL_MS];
    char IMS[BID][VEL_MS];
    int id = (int) threadId;

    fillMS(UMS, IMS);
    stvoriRadneDretve();
    stvoriIzlazneDretve();

    while (1) {
        int U = dohvatiUlaz(id);
        int T = obradiUlaz(U);

        UMS[T][0] = U;
        printf("U%d: dohvatiUlaz(%d) => '%c', obradiUlaz(%c) => %d\n", id, id, U, U, T);
        printUMS(UMS);
        printIMS(IMS);
        printf("\n");

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
    for (int i = 0; i < BRD; i++) {
        pthread_t radnaDretva;
        pthread_create(&radnaDretva, NULL, rad, i);
    }
}

void stvoriIzlazneDretve() {
    for (int i = 0; i < BID; i++) {
        pthread_t izlaznaDretva;
        pthread_create(&izlaznaDretva, NULL, izlaz, i);
    }
}