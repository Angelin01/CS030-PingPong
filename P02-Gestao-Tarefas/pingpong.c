#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include "pingpong.h"
#include "datatypes.h"

#define ERRSTACK -1

int idcounter = 1;

void pingpong_init() {
    setvbuf(stdout, 0, _IONBF, 0);
}

int task_create(task_t *task, void (*start_func)(void *), void *arg) {
    task->tid = idcounter++;
    getcontext(task->tContext);
    getcontext(task->mainContext);

    // Alocando stack
    task->stack = malloc(sizeof(char) * STACKSIZE);
    if(stack) {
        task->taskContext.uc_stack.ss_sp = task->stack;
        task->taskContext.uc_stack.ss_size = STACKSIZE;
        task->taskContext.uc_stack.ss_flags = 0;
        task->taskContext.uc_link = 0;
    }
    else {
        perror("Erro em task_create na criacao da stack: ");
        return (ERRSTACK);
    }

    makecontext(task->tContext, start_func, 1, arg);

    #ifdef DEBUG
    printf("task_create: criou tarefa %d", task->id);
    #endif
    return (task->tid);
}
