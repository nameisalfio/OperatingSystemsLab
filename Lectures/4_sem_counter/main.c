#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h> // sem_t, sem_init, sem_wait, sem_post, sem_destroy

/*
    CONSEGNA:

    Scrivere un programma che crea due thread che incrementano un contatore condiviso 100000 volte ciascuno.
    Devono essere generati due thread, ciascuno dei quali incrementa il contatore condiviso 100000 volte e la mutua esclusione
    deve essere garantita tramite un semaforo. Alla fine, il programma stampa il valore del contatore.        
*/

// Struttura dati condivisa tra i thread
typedef struct {
    sem_t semaphore;      // Semaforo per sincronizzare l'accesso alla risorsa condivisa
    int counter;  // Risorsa condivisa tra i thread
} shared_t;

// Struttura dati per ogni thread
typedef struct {
    pthread_t tid;        // ID del thread
    int thread_num;       // Numero del thread (identificativo)
    shared_t* shared;  // Puntatore alla struttura condivisa
} thread_t;

// Funzione eseguita dai thread per modificare la risorsa condivisa
void* increment_counter(void* arg) {
    
    thread_t* tdata = (thread_t*)arg;

    // Ogni thread incrementa il contatore 100000 volte
    for (int i = 0; i < 100000; i++) {
        // Attesa del semaforo prima di accedere alla risorsa condivisa
        sem_wait(&tdata->shared->semaphore);

        // Incrementa il contatore
        tdata->shared->counter++;

        // Rilascia il semaforo per consentire ad altri thread di accedere alla risorsa condivisa
        sem_post(&tdata->shared->semaphore);
    }

    printf("Thread %d ha incrementato la risorsa a %d\n", tdata->thread_num, tdata->shared->counter);
    pthread_exit(NULL);
}

int main() {
    // Struttura condivisa
    shared_t shared;
    shared.counter = 0;  // Inizializza la risorsa condivisa

    // Inizializzazione del semaforo con valore iniziale 1 (semaforo binario)
    if (sem_init(&shared.semaphore, 0, 1) != 0) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

    // Struttura per i dati dei thread
    thread_t threads[2];

    // Creazione e inizializzazione dei thread
    for (int i = 0; i < 2; i++) {
        threads[i].thread_num = i + 1;   // Assegna un numero al thread (1, 2, ...)
        threads[i].shared = &shared;     // Tutti i thread condividono la stessa struttura condivisa

        // Creazione del thread
        if (pthread_create(&threads[i].tid, NULL, increment_counter, (void*)&threads[i]) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    // Attesa che tutti i thread terminino
    for (int i = 0; i < 2; i++) {
        if (pthread_join(threads[i].tid, NULL) != 0) {
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }
    }

    // Distruzione del semaforo dopo l'utilizzo
    sem_destroy(&shared.semaphore);

    // Stampa del valore finale della risorsa condivisa
    printf("\nValore finale della risorsa condivisa: %d\n", shared.counter);

    return 0;
}