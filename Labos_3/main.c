#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#define BUD 6
#define BRD 3
#define BID 4
#define VEL_MS 5

char UMS[BRD][VEL_MS];
char IMS[BID][VEL_MS];

int ulazUMS[BRD];
int ulazIMS[BID];

int last_data[BID];

pthread_t threads[BUD + BRD + BID];
sem_t writeSem;
sem_t readUMSSem[BRD];
sem_t readIMSSem[BID];

//initialize threads
void stvoriUlazneDretve();
void stvoriRadneDretve();
void stvoriIzlazneDretve();

//thread functions
int dohvatiUlaz();
int obradiUlaz();
void obradi(int podatak, int* r);

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

    for (int i = 0; i < BID; i++) {
        sem_init(&readIMSSem[i], 0, 0);
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

void obradi(const int podatak, int* r) {
    *r = podatak + ('a' - 'A');
}

void *ulaz(void * threadId) {
    const int id = (int) threadId;

    while (1) {
        //K.O.
        sem_wait(&writeSem);
        const int U = dohvatiUlaz(id);
        const int T = obradiUlaz(id, U);

        UMS[T][ulazUMS[T]] = U;
        ulazUMS[T] = (ulazUMS[T] + 1) % VEL_MS;

        printf("U%d: dohvatiUlaz(%d) => '%c', obradiUlaz(%c) => %d\n", id, id, U, U, T);
        printUMS();
        printIMS();
        printf("\n");

        sleep(1);

        sem_post(&readUMSSem[T]);
        sem_post(&writeSem);
        //Kraj K.O.

        sleep(3);
    }
}

void *rad(void * threadId) {
    const int id = (int) threadId;

    int izlaz[BRD];

    //sleep prije prvog citanja
    sleep(5);

    while (1) {

        //K.O.
        sem_wait(&readUMSSem[id]);
        sem_wait(&writeSem);

        const int P = UMS[id][izlaz[id]];
        UMS[id][izlaz[id]] = '-';
        izlaz[id] = (izlaz[id] + 1) % VEL_MS;

        //obradi podatak
        int r;
        obradi(P, &r);
        const int t = (r + id) % BID;

        IMS[t][ulazIMS[t]] = r;
        ulazIMS[t] = (ulazIMS[t] + 1) % VEL_MS;

        printf("R%d: obradjujem UMS[%d] => '%c'\n", id, id, P);
        printUMS();
        printIMS();
        printf("\n");

        sleep(1);

        sem_post(&readIMSSem[t]);
        sem_post(&writeSem);
        //Kraj K.O.

        sleep(3);
    }
}

void *izlaz(void * threadId) {
    const int id = (int) threadId;

    int izlaz[BID];

    sem_wait(&readIMSSem[id]);

    while (1) {
        //K.O.
        sem_wait(&writeSem);

        const int data = IMS[id][izlaz[id]];
        if (data != '-') {
            last_data[id] = data;
            IMS[id][izlaz[id]] = '-';
            izlaz[id] = (izlaz[id] + 1) % VEL_MS;

            printf("I%d: ispisujem IMS[%d] => '%c'\n", id, id, last_data[id]);
            printUMS();
            printIMS();
        } else {
            printf("I%d: ispisujem zadnji poznati poznati podatak => %c\n", id, last_data[id]);
        }


        printf("\n");

        sleep(1);

        sem_post(&writeSem);
        //Kraj K.O.

        sleep(3);
    }
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