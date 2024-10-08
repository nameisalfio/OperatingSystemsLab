#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

// Macro per gestire gli errori
#define handle_error(msg)                               \
do {                                                    \
    fprintf(stderr, "[%d] error: %s\n", __LINE__, msg); \
    exit(EXIT_FAILURE);                                 \
} while(0);

/* 
Motivazione della macro:
La macro `handle_error` semplifica la gestione degli errori all'interno del codice.
1. Utilizza `fprintf` per stampare un messaggio di errore personalizzato, includendo anche il numero di linea (`__LINE__`), che rende più facile il debug.
2. Chiama `exit(EXIT_FAILURE)` per terminare il programma in modo pulito in caso di errore.
La macro elimina la necessità di scrivere ripetutamente lo stesso codice di controllo degli errori in ogni parte del programma.
*/

int main(int argc, char *argv[]) {
    // Controlla che il nome del file sia stato fornito come argomento
    if (argc < 2) handle_error("missing file name");

    // Dichiarazione delle variabili:
    // fd: file descriptor
    // info: struttura che conterrà le informazioni sul file (come la dimensione)
    // map: puntatore alla mappatura in memoria del file
    int fd;
    struct stat info;
    char *map;

    // Apre il file in modalità sola lettura
    // open() restituisce un file descriptor (fd). Se l'apertura fallisce, `open` restituisce un valore negativo.
    if ((fd = open(argv[1], O_RDONLY)) < 0) handle_error("open");

    // Recupera le informazioni sul file, tra cui la dimensione, usando fstat().
    // fstat() riempie la struttura `info` con i metadati del file. Se fallisce, gestiamo l'errore con `handle_error`.
    if (fstat(fd, &info) == -1) handle_error("fstat");

    // Mappa il file in memoria:
    // mmap() mappa il file in memoria, rendendo accessibili i suoi contenuti come se fossero un array in RAM.
    // Viene mappata la dimensione totale del file (`info.st_size`), con permessi di sola lettura (PROT_READ).
    // MAP_PRIVATE assicura che le modifiche alla mappatura non influenzino il file su disco.
    // Se `mmap` fallisce, restituisce `MAP_FAILED`, e l'errore viene gestito.
    if((map = mmap(NULL, info.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED) handle_error("mmap");

    // Chiude il file:
    // Il file descriptor può essere chiuso dopo aver mappato il file in memoria, poiché i dati sono accessibili tramite la mappatura.
    close(fd);

    // Stampa il contenuto del file mappato in memoria:
    // Scorriamo l'array `map`, stampando carattere per carattere i dati del file.
    // La dimensione del file è `info.st_size`, quindi il ciclo itera su tutti i byte del file.
    for(int i = 0; i < info.st_size; i++) {
        printf("%c", map[i]);
    }

    // Smette di utilizzare la mappatura:
    // Dopo aver terminato l'uso della mappatura, `munmap()` viene utilizzato per liberare la memoria associata alla mappatura.
    // Se `munmap()` fallisce, gestiamo l'errore con `handle_error`.
    if (munmap(map, info.st_size) == -1) handle_error("munmap");

    return 0;
}
