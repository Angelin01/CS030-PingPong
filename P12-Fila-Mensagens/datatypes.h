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
    exited,
    sleeping
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
    int userTask;

    // Prioridades
    int staticPrio;
    int dynamicPrio;

    // Timer
    unsigned int startTime;
    unsigned int cpuTime;
    unsigned int activations;

    // Suspensao
    struct task_t* suspendedQueue;
    int exitCode;

    // Sleep
    int wakeTime;

} task_t ;

// estrutura que define um semáforo
typedef struct {
    int value;

  // Suspensao
    struct task_t* suspendedQueue;
    int active;
} semaphore_t ;

// estrutura que define um mutex
typedef struct
{
  // preencher quando necessário
} mutex_t ;

// estrutura que define uma barreira
typedef struct {
    int maxTasks;
    int numTasks;

    // Suspensao
    struct task_t* suspendedQueue;
    int active;
} barrier_t ;

// estrutura que define uma fila de mensagens
typedef struct {
    // Parametros da fila
    int msgSize;
    int numMsg;

    // Semaforos internos
    semaphore_t s_vaga;
    semaphore_t s_item;
    semaphore_t s_buffer;

    // Fila em si
    void* buffer;

    // Flag de ativo
    int active;
} mqueue_t ;

#endif
