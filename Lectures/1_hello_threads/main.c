#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> // pthread_t, pthread_create, pthread_join

/*
    CONSEGNA:

    Scrivere un programma che crea un thread che stampa "Hello, Threads!".        
*/

void* print_message(void* arg) {
    printf("Thread %d: Hello, Threads!\n", *(int*)arg);
    pthread_exit(NULL);
}

int main() {

    // Il tipo pthread_t è un tipo che rappresenta un thread
    pthread_t thread;
    int thread_id = 1;
    
    // Creazione del thread
    if (pthread_create(&thread, NULL, print_message, (void*)&thread_id) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    /* Parametri della funzione pthread_create:
     * 1. thread: puntatore a una variabile di tipo pthread_t
     * 2. NULL: attributi del thread (NULL = default)
     * 3. print_message: puntatore alla funzione da eseguire nel thread
     * 4. (void*)&thread_id: argomento da passare alla funzione (in questo caso stiamo passando il thread_id).
     * Ogni thread ha un proprio spazio di memoria, quindi è possibile passare un puntatore a una variabile locale.
     * 
     * Nota: la funzione passata deve avere il seguente prototipo:
     * void* funzione(void* arg);
     * deve restituire un puntatore a void e accettare un puntatore a void come argomento
    */
    
    // Attesa che il thread termini
    if (pthread_join(thread, NULL) != 0) {
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }
    
    printf("Main thread finished\n");
    return 0;
}