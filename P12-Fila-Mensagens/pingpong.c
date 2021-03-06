#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>
#include "pingpong.h"
#include "datatypes.h"
#include "queue.h"

#define ERRSTACK -10 // Comecar os erros mais para tras pq sim
#define ERRSIGNAL -11
#define ERRTIMER -12
#define QUANTUM 20

// Valores num�ricos
long idcounter = 1;
volatile short ticksToGo;
volatile unsigned int miliTime = 0;

// Para tasks
task_t mainTask;
task_t* currentTask;
task_t dispatcher;
task_t* taskQueue;
task_t* toFree;
task_t* sleepQueue;

// Preemp��o
struct sigaction quantumCheck;
struct itimerval quantumTimer;
int preempcaoAtiva;

// Pr�-declara��es
void dispatcher_body();
void quantum_handler();

void pingpong_init() {
    #ifdef DEBUG
    printf("Inicializando pingpong\n");
    #endif
    setvbuf(stdout, 0, _IONBF, 0);

    // Para preemp��o
    quantumCheck.sa_handler = quantum_handler;
    sigemptyset(&quantumCheck.sa_mask);
    quantumCheck.sa_flags = 0;
    if(sigaction(SIGALRM, &quantumCheck, 0) < 0) {
        perror("Erro no sigaction: ");
        exit(ERRSIGNAL);
    }

    quantumTimer.it_value.tv_sec = 0;
    quantumTimer.it_interval.tv_sec = 0;
    quantumTimer.it_value.tv_usec = 1000; // 1000us = 1ms
    quantumTimer.it_interval.tv_usec = 1000;

    if(setitimer (ITIMER_REAL, &quantumTimer, 0) < 0) {
        perror("Erro no setitimer: ");
        exit(ERRTIMER);
    }
    preempcaoAtiva = 1;

    /* -------------- */
    /* Coisas da main */
    /* -------------- */
    #ifdef DEBUG
    printf("Inicilizando tarefa main\n");
    #endif // DEBUG

    // ID e contexto
    mainTask.tid = 0;

    getcontext(&(mainTask.tContext));

    // Coisas de stack
    mainTask.stack = malloc(sizeof(char) * SIZESTACK);
    mainTask.tContext.uc_stack.ss_sp = mainTask.stack;
    mainTask.tContext.uc_stack.ss_size = SIZESTACK;
    mainTask.tContext.uc_stack.ss_flags = 0;
    mainTask.tContext.uc_link = 0;

    mainTask.userTask = 1; // A partir do projeto 7, main eh tarefa de usuario

    getcontext(&(mainTask.tContext));
    currentTask = &mainTask;
    queue_append((queue_t**)&taskQueue, (queue_t*)&mainTask);
    mainTask.currentQueue = &taskQueue;

    /* -------------------- */
    /* Coisas do dispatcher */
    /* -------------------- */
    task_create(&dispatcher, dispatcher_body, NULL);
    dispatcher.userTask = 0;
	dispatcher.activations = 0;
    queue_remove((queue_t**)&taskQueue, (queue_t*)&dispatcher);
    dispatcher.currentQueue = NULL;
    task_switch(&dispatcher);
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


    queue_append((queue_t**)&taskQueue, (queue_t*)task);
    task->currentQueue = &taskQueue; // Para suspend no futuro
    task->state = ready;
    task->staticPrio = 0;
    task->dynamicPrio = 0;
    task->cpuTime = 0;
    task->activations = 0;
    task->startTime = systime();
    task->userTask = 1;

    task->suspendedQueue = NULL;

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
    task_t* toResume = currentTask->suspendedQueue;
    task_t* auxResume;

    printf("Task %d exit: execution time %u ms, processor time %u ms, %u activations\n", currentTask->tid, miliTime - currentTask->startTime, currentTask->cpuTime, currentTask->activations);

    if(currentTask->suspendedQueue) {
        auxResume = toResume->next;
        while(auxResume != toResume) {
            task_resume(toResume);
            toResume = auxResume;
            auxResume = auxResume->next;
        }
        task_resume(toResume);
    }

    /* Isso parece meio porco, procurar solucao melhor */
    if(currentTask == &dispatcher) { // Para distinguir se quem esta saindo eh o dispatcher...
        currentTask = &mainTask;
        swapcontext(&(tempTask->tContext), &(mainTask.tContext));
    }
    else { // Ou tarefas normais
        currentTask = &dispatcher;
        toFree = tempTask;
        tempTask->state = exited;
        tempTask->exitCode = exitCode;
        swapcontext(&(tempTask->tContext), &(dispatcher.tContext));
    }
}

int task_id() {
    return(currentTask->tid);
}

/* -------------------- */
/*      Dispatcher      */
/* -------------------- */

task_t* scheduler() {
    if(!taskQueue) { // Nao ha tarefas
        return(NULL);
    }

    task_t* highestPriority = taskQueue;
    task_t* aux = taskQueue;

    do {
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

    // Envelhecimento separado
    aux = taskQueue;
    do {
        if(aux->dynamicPrio > -20) { // Envelhece a tarefa se puder
            aux->dynamicPrio--;
        }
		aux = aux->next;
    } while(aux != taskQueue);

    highestPriority->dynamicPrio = highestPriority->staticPrio; // Reseta a prioridade
    return(highestPriority);
}

void dispatcher_body() {
    task_t* next;
    task_t* toWake;
    task_t* auxWake;
	unsigned int currentTime;

    while(taskQueue || sleepQueue) { // Se fila estiver vazia, ACAAABOOOO
		dispatcher.activations++;

		// Verifica se deve acordar tarefas
		if(sleepQueue) {
            auxWake = sleepQueue;
			currentTime = miliTime;
            do {
                toWake = auxWake;
                auxWake = auxWake->next;
                if(toWake->wakeTime <= currentTime) { // Se passou da hora de acordar
                    task_resume(toWake);
                }
            } while (sleepQueue && auxWake != sleepQueue);
		}

        next = scheduler(); // NULL se a fila est� vazia
        if(next) { // Apenas garantia
            queue_remove((queue_t**)&taskQueue, (queue_t*)next);
            next->currentQueue = NULL;
            next->activations++;
            ticksToGo = QUANTUM;
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

    // Volta pra pronta se estava executando
    queue_append((queue_t**)&taskQueue, (queue_t*)currentTask);
    currentTask->currentQueue = &taskQueue;
    currentTask->state = ready;

    task_switch(&dispatcher);
}

/* ------------------- */
/*      Suspensas      */
/* ------------------- */
void task_suspend(task_t *task, task_t **queue) {
    preempcaoAtiva = 0;
    task = !task ? currentTask : task; // Se nulo eh task atual

    #ifdef DEBUG
    printf("task_suspend: suspendendo task %d\n", task->tid);
    #endif

    if(queue) { // Se queue esta setado, remover task da queue
        if(task->currentQueue) {
            queue_remove((queue_t**)task->currentQueue, (queue_t*)task);
        }
        queue_append((queue_t**)queue, (queue_t*)task);
        task->currentQueue = queue;
    }

    // Task agora esta suspensa
    task->state = suspended;
    preempcaoAtiva = 1;
    task_switch(&dispatcher);
}

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
	task->state=ready;
}

int task_join(task_t *task) {
    preempcaoAtiva = 0;
    // Se nao existir ou ja tiver saido, retorna -1
    if(!task || task->state == exited) {
        return (-1);
    }

    #ifdef DEBUG
    printf("Dando join na tarefa %d com a tarefa %d\n", currentTask->tid, task->tid);
    #endif // DEBUG

    task_suspend(NULL, &(task->suspendedQueue)); // Suspende e soh volta quando task voltar
    return (task->exitCode); // Quando voltar a tarefa tera saido e tera um exitCode
}

/* --------------------- */
/*      Prioridades      */
/* --------------------- */
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

/* ----------------- */
/*      Quantum      */
/* ----------------- */
void quantum_handler() {
    currentTask->cpuTime++;
    miliTime++;

    // Se n�o for as tasks principais e acabar o quantum
    if(currentTask->userTask && (--ticksToGo) <= 0 && preempcaoAtiva) {
        #ifdef DEBUG
        printf("quantum_handler: acabou quantum da task %d\n", currentTask->tid);
        #endif // DEBUG
        task_yield();
    }
}

/* -------------- */
/*      Timer     */
/* -------------- */
unsigned int systime() {
    return miliTime;
}

/* -------------- */
/*      Sleep     */
/* -------------- */
void task_sleep(int t) {
    preempcaoAtiva = 0;
    #ifdef DEBUG
    printf("task_sleep: dormindo task %d por %d segundos\n", currentTask->tid, t);
    #endif

    if (t <= 0){
        preempcaoAtiva = 1;
        return;
    }

    currentTask->wakeTime = miliTime + t*1000; // t em segundos

    if(currentTask->currentQueue) {
        queue_remove((queue_t**)&currentTask->currentQueue, (queue_t*)currentTask);
    }
    queue_append((queue_t**)&sleepQueue, (queue_t*)currentTask);

    // Task agora esta dormindo
    currentTask->currentQueue = &sleepQueue;
    currentTask->state = sleeping;
    preempcaoAtiva = 1;
    task_switch(&dispatcher);
}

/* ----------------- */
/*     Semaforos     */
/* ----------------- */

int sem_create(semaphore_t *s, int value) {
    #ifdef DEBUG
    printf("Criando sem�foro com valor %d\n", value);
    #endif // DEBUG

    if(!s || s->active) {
        return(-1);
    }

    s->value = value;
    s->active=1;
    return(0);
}

int sem_down(semaphore_t *s){
    preempcaoAtiva = 0;
    // Checa se semaforo existe
    if(!s || !s->active) {
        preempcaoAtiva = 1;
        return(-1);
    }
    #ifdef DEBUG
    printf("sem_down: Tarefa %d dando down, para valor %d\n", currentTask->tid, s->value-1);
    #endif // DEBUG


    if(--(s->value) < 0) {
        task_suspend(NULL, &s->suspendedQueue);
        // suspend religa preempcao no final
    }

    preempcaoAtiva = 1;
    // Retornar -1 se semaforo foi destruido
    if(!s->active) {
        return(-1);
    }
    return(0);
}

int sem_up(semaphore_t *s) {
    preempcaoAtiva = 0;
    if(!s || !s->active) {
        preempcaoAtiva = 1;
        return(-1);
    }

    #ifdef DEBUG
    printf("sem_up: Tarefa %d dando up, para valor %d\n", currentTask->tid, s->value+1);
    #endif // DEBUG

    if(++(s->value) <= 0) { // Acordar tarefa se ainda houveram tarefas esperando
        if(s->suspendedQueue) { // Soh por garantia, deve haver uma tarefa na fila
            task_resume(s->suspendedQueue);
        }
    }
    preempcaoAtiva = 1;
    return(0);
}

int sem_destroy(semaphore_t *s) {
    preempcaoAtiva = 0;
    if(!s || !s->active) {
        preempcaoAtiva = 1;
        return(-1);
    }

    // Acorda todas as tarefas que tem que acordar
    // Bem melhor que a implementacao do task_exit, mas nao vou mudar la
    while(s->suspendedQueue) {
        task_resume(s->suspendedQueue);
    }

    s->active = 0;
    preempcaoAtiva = 1;
    return(0);
}

/* ----------------- */
/*     Barreiras     */
/* ----------------- */

int barrier_create(barrier_t* b, int N) {
    if(!b || N <= 0 || b->active) {
        return(-1);
    }
    #ifdef DEBUG
    printf("Criando barreira com limite %d\n", N);
    #endif // DEBUG

    b->maxTasks = N;
    b->numTasks = 0;
    b->active = 1;
    return(0);
}

int barrier_join(barrier_t* b) {
    preempcaoAtiva = 0;
    if(!b || !b->active) {
        preempcaoAtiva = 1;
        return(-1);
    }
    #ifdef DEBUG
    printf("Task %d parando em barreira\n", task_id());
    #endif // DEBUG

    // Se chegou ao limite de tasks na barreira
    if(b->numTasks >= b->maxTasks - 1) {
        while(b->suspendedQueue) {
            task_resume(b->suspendedQueue);
        }
        // Renicializa barreira
        b->numTasks = 0;
        preempcaoAtiva = 1;
        return(0);
    }

    // Se nao chegou ao limite de tasks
    ++b->numTasks;
    task_suspend(NULL, &b->suspendedQueue); // Religa preempcao internamente
    // Se barreira for destruida, retornar -1
    if(!b) {
        return(-1);
    }
    return(0);
}

int barrier_destroy(barrier_t* b) {
    preempcaoAtiva = 0;
    if(!b || !b->active) {
        preempcaoAtiva = 1;
        return(-1);
    }

    // Acorda todas as tasks
    while(b->suspendedQueue) {
        task_resume(b->suspendedQueue);
    }
    b->numTasks = 0;
    // Desliga barreira
    b->active = 0;

    preempcaoAtiva = 1;
    return(0);
}

/* ----------------- */
/*     Mensagens     */
/* ----------------- */

int mqueue_create(mqueue_t* queue, int max, int size) {
    if(!queue || queue->active) {
        return(-1);
    }
    if(max <= 0 || size <= 0) {
        return(-1);
    }

    #ifdef DEBUG
    printf("Criando mqueue de tamanho %d com mensagens de %d bytes\n", max, size)
    #endif // DEBUG

    // Seta parametros da mqueue
    queue->msgSize = size;
    queue->numMsg = 0;

    // Semaforos internos
    if(sem_create(&queue->s_buffer, 1) != 0) {
        return(-1);
    }
    if(sem_create(&queue->s_vaga, max) != 0) {
        return(-1);
    }
    if(sem_create(&queue->s_item, 0) != 0) {
        return(-1);
    }

    // Aloca vetor de tamanho (msgSize * maxMsgs) bytes
    queue->buffer = malloc(size * max);
    if(!queue->buffer) { // Pode ficar sem memoria, suponho
        return(-1);
    }

    queue->active = 1;
    return(0);
}

int mqueue_send(mqueue_t* queue, void* msg) {
    if(!queue || !queue->active || !msg) {
        return(-1);
    }

    // Devem ter wrap pois semaforo pode ser destruido no destruir queue
    if(sem_down(&queue->s_vaga) != 0) {
        return(-1);
    }
    if(sem_down(&queue->s_buffer) != 0) {
        return(-1);
    }

    //     Ponteiro      + Offset do "vetor de msg"
    memcpy(queue->buffer + queue->numMsg*queue->msgSize, msg, queue->msgSize);
    ++queue->numMsg;

    // Devem ter wrap pois semaforo pode ser destruido no destruir queue
    if(sem_up(&queue->s_buffer) != 0) {
        return(-1);
    }
    if(sem_up(&queue->s_item) != 0) {
        return(-1);
    }

    return(0);
}

int mqueue_recv(mqueue_t* queue, void* msg) {
    if(!queue || !queue->active || !msg) {
        return(-1);
    }

    // Devem ter wrap pois semaforo pode ser destruido no destruir queue
    if(sem_down(&queue->s_item) != 0) {
        return(-1);
    }
    if(sem_down(&queue->s_buffer) != 0) {
        return(-1);
    }

    memcpy(msg, queue->buffer, queue->msgSize);
    --(queue->numMsg);
    memmove(queue->buffer, queue->buffer + queue->msgSize, queue->numMsg * queue->msgSize); // memcpy nao seguro para overlap

    if(sem_up(&queue->s_buffer) != 0) {
        return(-1);
    }
    if(sem_up(&queue->s_vaga) != 0) {
        return(-1);
    }

    return(0);
}

int mqueue_msgs(mqueue_t* queue) {
    if(!queue || !queue->active) {
        return(-1);
    }

    return(queue->numMsg);
}

int mqueue_destroy(mqueue_t* queue) {
    // Tem que desligar preempcao parar evitar que uma tarefa tente escrever no meio da destruicao
    preempcaoAtiva = 0;
    if(!queue || !queue->active) {
        preempcaoAtiva = 1;
        return(-1);
    }

    // Desaloca buffer
    free(queue->buffer);

    // Destroi semaforos
    if(sem_destroy(&queue->s_buffer) != 0) {
         preempcaoAtiva = 1;
         return(-1);
    }
    if(sem_destroy(&queue->s_item) != 0) {
         preempcaoAtiva = 1;
         return(-1);
    }
    if(sem_destroy(&queue->s_vaga) != 0) {
         preempcaoAtiva = 1;
         return(-1);
    }

    queue->numMsg = 0;

    // Desliga queue
    queue->active = 0;
    preempcaoAtiva = 1;
    return(0);
}
