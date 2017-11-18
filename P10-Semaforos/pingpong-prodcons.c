#include <stdio.h>
#include <stdlib.h>
#include "pingpong.h"
#include "queue.h"

#define NUMACOES 5
#define NUMPROD 3
#define NUMCONS 2

typedef struct buffer {
    struct buffer* prev;
    struct buffer* next;

    int value;
}

samephore_t s_vaga;
samephore_t s_buffer;
samephore_t s_item;

task_t task_prod[NUMPROD];
task_t task_cons[NUMCONS];

void produtor(int id) {
    int i, produzido;

    for(i = 0; i < NUMACOES; ++i) {
        task_sleep(1);

        sem_down(&s_vaga);
        sem_down(&s_buffer);

        // Insere elemento
        printf("p%d produziu %d\n", id, produzido);

        sem_up(&s_buffer);
        sem_up(&s_item);
    }
}

void consumidor(int id) {
    int i, consumido;

    for(i = 0; i < NUMACOES; ++i) {
        task_sleep(1);

        sem_down(&s_item);
        sem_down(&s_buffer);

        // Retira elemento
        printf("c%d consumiu %d\n", id, consumido);

        sem_up(&s_buffer);
        sem_up(&s_vaga);
    }
}

int main() {
    int i;

    sem_create(&s_buffer, 1);
    sem_create(&s_vaga, 2);
    sem_create(&s_item, 5);

    for(i = 0; i < NUMPROD, ++i) {
        task_create(&task_prod[i], produtor, i);
    }
    for(i = 0; i < NUMCONS, ++i) {
        task_create(&task_cons[i], consumidor, i);
    }

    for(i = 0; i < NUMPROD, ++i) {
        task_join(&task_prod[i]);
    }
    for(i = 0; i < NUMCONS, ++i) {
        task_join(&task_cons[i]);
    }

    sem_destroy(&s_buffer);
    sem_destroy(&s_vaga);
    sem_destroy(&s_item);

    task_exit(0);
    exit(0);
}
