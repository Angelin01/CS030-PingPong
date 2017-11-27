// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DAINF UTFPR
// Versão 1.0 -- Março de 2015
//
// interface do driver de disco rígido

#ifndef __DISKDRIVER__
#define __DISKDRIVER__
#include "datatypes.h"

typedef struct disk_request {
    // Filas
    struct disk_request* prev;
    struct disk_request* next;

    // Tarefa que fez o pedido
    task_t* task;

    // Parametros da request
    int operation; // Read ou write (usar os defines ja prontos)
    void* buffer;
    int block;
} disk_request;

// structura de dados que representa o disco para o SO
typedef struct {
	// Semaforo do disco
	semaphore_t s_disk;

	// Tarefas esperando disco e requisicoes
	task_t* suspendedQueue;
	disk_request* requestQueue;

	// Parametros do disco
	int numBlocks;
	int blockSize;

	// Flags do disco
	int opComplete;
} disk_t;

// inicializacao do driver de disco
// retorna -1 em erro ou 0 em sucesso
// numBlocks: tamanho do disco, em blocos
// blockSize: tamanho de cada bloco do disco, em bytes
int diskdriver_init (int *numBlocks, int *blockSize) ;

// leitura de um bloco, do disco para o buffer indicado
int disk_block_read (int block, void *buffer) ;

// escrita de um bloco, do buffer indicado para o disco
int disk_block_write (int block, void *buffer) ;

#endif
