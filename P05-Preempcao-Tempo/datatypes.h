// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DAINF UTFPR
// Versão 1.0 -- Março de 2015
//
// Estruturas de dados internas do sistema operacional

#include <ucontext.h>

#ifndef __DATATYPES__
#define __DATATYPES__

#define SIZESTACK 32768

enum state_t {
    executing = 0,
    ready,
    suspended,
    exited
};

// Estrutura que define uma tarefa
typedef struct task_t {
    // Para filas
    struct task_t* prev;
    struct task_t* next;

    struct task_t** currentQueue;

    // Para voltar pro main
    struct task_t* tmain;

    // Para tarefa em si
    int tid;
    ucontext_t tContext;
    char* stack;
    enum state_t state;

    // Prioridades
    int staticPrio;
    int dynamicPrio;

} task_t ;

// estrutura que define um semáforo
typedef struct
{
  // preencher quando necessário
} semaphore_t ;

// estrutura que define um mutex
typedef struct
{
  // preencher quando necessário
} mutex_t ;

// estrutura que define uma barreira
typedef struct
{
  // preencher quando necessário
} barrier_t ;

// estrutura que define uma fila de mensagens
typedef struct
{
  // preencher quando necessário
} mqueue_t ;

#endif
