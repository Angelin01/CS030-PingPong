#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include "pingpong.h"
#include "datatypes.h"
#include "queue.h"

#define ERRSTACK -10 // Comecar os erros mais para tras pq sim

long idcounter = 1;

task_t mainTask;
task_t* currentTask;
task_t* dispatcher;
task_t* taskQueue;
task_t* toFree;

void dispatcher_body();

void pingpong_init() {
    #ifdef DEBUG
    printf("Inicializando pingpong\n");
    #endif
    setvbuf(stdout, 0, _IONBF, 0);

    // ID e contexto
    mainTask.tid = 0;

    getcontext(&(mainTask.tContext));

    // Coisas de stack
    mainTask.stack = malloc(sizeof(char) * SIZESTACK);
    mainTask.tContext.uc_stack.ss_sp = mainTask.stack;
    mainTask.tContext.uc_stack.ss_size = SIZESTACK;
    mainTask.tContext.uc_stack.ss_flags = 0;
    mainTask.tContext.uc_link = 0;

    getcontext(&(mainTask.tContext));
    currentTask = &mainTask;

    // Aloca dispatcher
    dispatcher = malloc(sizeof(task_t));
    // Cria o dispatcher
    task_create(dispatcher, dispatcher_body, NULL);
    queue_remove((queue_t**)&taskQueue, (queue_t*)dispatcher);
    dispatcher->currentQueue = NULL;
}

int task_create(task_t *task, void (*start_func)(void *), void *arg) {
    task->tid = idcounter++;
    getcontext(&(task->tContext));

    // Alocando stack
    task->stack = malloc(sizeof(char) * SIZESTACK);
    if(task->stack) {
        task->tContext.uc_stack.ss_sp = task->stack;
        task->tContext.uc_stack.ss_size = SIZESTACK;
        task->tContext.uc_stack.ss_flags = 0;
        task->tContext.uc_link = 0;
    }
    else {
        perror("Erro em task_create na criacao da stack: ");
        return (ERRSTACK);
    }

    // Seta task principal
    task->tmain = &mainTask;
    if(arg) { // Chek se tem que passar argumento ou nao
        makecontext(&(task->tContext), (void*) start_func, 1, arg);
    }
    else {
        makecontext(&(task->tContext), (void*) start_func, 0);
    }

    queue_append((queue_t**)&taskQueue, (queue_t*)task);
    task->currentQueue = &taskQueue; // Para suspend no futuro
    task->state = ready;
    task->staticPrio = 0;
    task->dynamicPrio = 0;

    #ifdef DEBUG
    printf("task_create: criou task %d\n", task->tid);
    #endif
    return (task->tid);
}

int task_switch(task_t *task) {
    #ifdef DEBUG
    printf("task_switch: trocando task %d -> %d \n", currentTask->tid, task->tid);
    #endif
    task_t* tempTask = currentTask;
    currentTask = task;
    task->state = executing;

    return(swapcontext(&(tempTask->tContext), &(task->tContext)));
}

void task_exit(int exitCode) {
    #ifdef DEBUG
    printf("task_exit: encerrando task %d\n", currentTask->tid);
    #endif
    task_t* tempTask = currentTask;
    /* Isso parece meio porco, procurar solucao melhor */
    if(currentTask == dispatcher) { // Para distinguir se quem esta saindo eh o dispatcher...
        currentTask = &mainTask;
        swapcontext(&(tempTask->tContext), &(mainTask.tContext));
    }
    else { // Ou tarefas normais
        currentTask = dispatcher;
        toFree = tempTask;
        tempTask->state = exited;
        swapcontext(&(tempTask->tContext), &(dispatcher->tContext));
    }
}

int task_id() {
    return(currentTask->tid);
}

/* -------------------- */
/*      Dispatcher      */
/* -------------------- */

task_t* scheduler() { // Implementar melhor com prioridades
    if(!taskQueue) { // Nao ha tarefas
        return(NULL);
    }

    task_t* highestPriority = taskQueue;
    task_t* aux = taskQueue;

    do {
        if(aux->dynamicPrio > -20) { // Envelhece a tarefa se puder
            aux->dynamicPrio--;
        }
        if(aux->dynamicPrio < highestPriority->dynamicPrio) {
            highestPriority = aux;
        }
        else if(aux->dynamicPrio == highestPriority->dynamicPrio) {
            if(aux->staticPrio < highestPriority->staticPrio) {
                highestPriority = aux;
            }
        }
        aux = aux->next;
    } while(aux != taskQueue);

    highestPriority->dynamicPrio = highestPriority->staticPrio; // Reseta a prioridade
    return(highestPriority);
}

void dispatcher_body() {
    task_t* next;

    while(queue_size((queue_t*)taskQueue) > 0) { // Se fila estiver vazia, ACAAABOOOO
        next = scheduler(); // NULL se a fila está vazia
        if(next) { // Apenas garantia
            queue_remove((queue_t**)&taskQueue, (queue_t*)next);
            next->currentQueue = NULL;
            task_switch(next);
            if(toFree) { // Se a task deu exit, precisa desalocar a stack
                free(toFree->stack);
                toFree = NULL;
            }
        }
    }

    task_exit(0);
}

void task_yield() {
    #ifdef DEBUG
    printf("task_yield: rendendo task %d\n", currentTask->tid);
    #endif
    if(currentTask != &mainTask) { // main task nao fica na fila
        queue_append((queue_t**)&taskQueue, (queue_t*)currentTask);
        currentTask->currentQueue = &taskQueue;
    }
    currentTask->state = ready;
    task_switch(dispatcher);
}

// Para o futuro
void task_suspend(task_t *task, task_t **queue) {
    task = !task ? currentTask : task; // Se nulo eh task atual

    #ifdef DEBUG
    printf("task_suspend: suspendendo task %d\n", task->tid);
    #endif

    if(queue) { // Se queue esta setado, remover task da queue
        queue_append((queue_t**)queue, queue_remove((queue_t**)task->currentQueue, (queue_t*)task));
        task->currentQueue = queue;
    }
    // Task agora esta suspensa
    task->state = suspended;
}

// Para o futuro
void task_resume(task_t *task) {
    #ifdef DEBUG
    printf("task_resume: resumindo task %d\n", task->tid);
    #endif

    if(task->currentQueue) { // Tirar somente se tiver uma queue
        queue_remove((queue_t**)task->currentQueue, (queue_t*)task);
    }
    // Volta a fila normal
    queue_append((queue_t**)&taskQueue, (queue_t*)task);
    task->currentQueue = &taskQueue;
}

// Prioridades
void task_setprio(task_t *task, int prio) {
    task = !task ? currentTask : task; // Se nulo eh task atual
    #ifdef DEBUG
    printf("task_setprio: atualizando prioridade da task %d para %d\n", task->tid, prio);
    #endif
    if(prio > 20 || prio < -20) {
        printf("Erro: prioridades devem ficar entre +20 e -20\n");
        return;
    }
    task->staticPrio = prio;
    task->dynamicPrio = prio;
}

int task_getprio(task_t *task) {
    task = !task ? currentTask : task; // Se nulo eh task atual

    return(task->staticPrio);
}
