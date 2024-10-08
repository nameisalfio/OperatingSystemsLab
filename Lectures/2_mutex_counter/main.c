#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/*
    CONSEGNA:

    Scrivere un programma che crea due thread che incrementano un contatore condiviso 100000 volte ciascuno.
    Devono essere generati due thread, ciascuno dei quali incrementa il contatore condiviso 100000 volte e la mutua esclusione
    deve essere garantita tramite un mutex. Alla fine, il programma stampa il valore del contatore.        
*/

// Struttura dati per le risorse condivise
typedef struct {
    pthread_mutex_t mutex;  // Mutex per gestire l'accesso alla variabile condivisa
    int counter;            // Variabile condivisa tra i thread
} shared_t;

// Struttura dati per i thread
typedef struct {
    pthread_t tid;          // ID del thread
    int thread_num;         // Numero del thread (identificativo)
    shared_t* shared;  // Puntatore alla struttura condivisa
} thread_t;

// Funzione eseguita dai thread per incrementare il contatore
void* increment_counter(void* arg) {
    // Casting dell'argomento ricevuto come puntatore alla struttura del thread (IMPORTANTISSIMO)
    thread_t* tdata = (thread_t*)arg;

    // Ogni thread incrementa il contatore 100000 volte
    for (int i = 0; i < 100000; i++) {
        // Blocca il mutex per garantire l'accesso esclusivo alla variabile condivisa
        pthread_mutex_lock(&tdata->shared->mutex);

        // Incrementa il contatore
        tdata->shared->counter++;

        // Sblocca il mutex per consentire l'accesso ad altri thread
        pthread_mutex_unlock(&tdata->shared->mutex);
    }

    // printf fuori dalla zona critica
    printf("[T%d] finished, counter: %d\n", tdata->thread_num, tdata->shared->counter);
    pthread_exit(NULL);
}

int main() {
    // Dichiarazione della struttura condivisa  
    shared_t shared;
    shared.counter = 0;  

    // Inizializzazione del mutex nella struttura condivisa
    if (pthread_mutex_init(&shared.mutex, NULL) != 0) {
        perror("pthread_mutex_init");
        exit(EXIT_FAILURE);
    }

    // Dichiarazione della struttura per i dati dei thread
    thread_t threads[2];


    for (int i = 0; i < 2; i++) {
        // Inizializzazione dei dati per ciascun thread
        threads[i].thread_num = i + 1;  // Assegna un numero al thread (1, 2, ...)
        threads[i].shared = &shared;  // Puntatore alla struttura condivisa

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

    // Distruzione del mutex nella struttura condivisa
    pthread_mutex_destroy(&shared.mutex);

    printf("\nValore finale del contatore: %d\n", shared.counter);
    return 0;
}

/*
    Perché non vedi il valore intermedio atteso (100000) alla fine del primo thread?

    Il valore del contatore non viene necessariamente aggiornato dopo che il Thread 2 ha completato il suo ciclo. 
    Ciò è dovuto al fatto che entrambi i thread stanno eseguendo contemporaneamente il codice. La concorrenza in 
    questo caso fa sì che i due thread incrementino la variabile counter in modo interlacciato.

    Quando il Thread 2 termina e stampa il suo messaggio con il valore del contatore, Thread 1 potrebbe aver già 
    eseguito diverse iterazioni del suo ciclo di incremento. Questo spiega perché quando Thread 2 stampa il valore 
    del contatore, non ottieni esattamente il risultato di 100000, ma un valore maggiore.

    Cosa accade in dettaglio?

    Entrambi i thread iniziano a eseguire il ciclo di incremento contemporaneamente.
    Il mutex garantisce che un solo thread alla volta possa incrementare il contatore. Tuttavia, i due thread si 
    alternano frequentemente nell'accedere alla risorsa condivisa, incrementando il contatore.

    Quando il Thread 2 termina e stampa il suo messaggio, Thread 2 potrebbe aver già fatto progressi significativi 
    nell'incrementare il contatore. Di conseguenza, quando il Thread 2 stampa il contatore, il valore non è 
    necessariamente 100000, perché il Thread 1 potrebbe aver già eseguito molte operazioni di incremento.
*/