#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include "pingpong.h"
#include "datatypes.h"

#define ERRSTACK -1

long idcounter = 1;

task_t mainTask;
task_t* currentTask;

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

    currentTask = &mainTask;
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

    makecontext(&(task->tContext), (void*) start_func, 1, arg);

    #ifdef DEBUG
    printf("task_create: criou tarefa %d\n", task->tid);
    #endif
    return (task->tid);
}

int task_switch(task_t *task) {
    #ifdef DEBUG
    printf("task_switch: trocando task %d -> %d \n", currentTask->tid, task->tid);
    #endif
    task_t* tempTask = currentTask;
    currentTask = task;

    return(swapcontext(&(tempTask->tContext), &(task->tContext)));
}

void task_exit(int exitCode) {
    #ifdef DEBUG
    printf("task_exit: encerrando task %d\n", currentTask->tid);
    #endif
    task_t* tempTask = currentTask;
    currentTask = &mainTask;
    swapcontext(&(tempTask->tContext), &(mainTask.tContext));

    return;
}

int task_id() {
    return(currentTask->tid);
}


