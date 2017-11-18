#include <stdio.h>
#include <stdlib.h>
#include "pingpong.h"
#include "queue.h"

#define NUMACOES 5

typedef struct buffer {
    struct buffer* prev;
    struct buffer* next;

    int value;
}

samephore_t s_vaga;
samephore_t s_buffer;
samephore_t s_item;

task p1, p2, p3, c1, c2;

void produtor(void* blah) {
    int i;

    for(i = 0; i < NUMACOES; ++i) {
        task_sleep(1);

        sem_down(&s_vaga);
        sem_down(&s_buffer);

        // Insere elemento

        sem_up(&s_buffer);
        sem_up(&s_item);
    }
}

void consumidor(void* bleh) {
    int i;

    for(i = 0; i < NUMACOES; ++i) {
        task_sleep(1);

        sem_down(&s_item);
        sem_down(&s_buffer);

        // Retira elemento

        sem_up(&s_buffer);
        sem_up(&s_vaga);
    }
}

int main() {
    sem_create(&s_buffer, 1);
    sem_create(&s_vaga, 2);
    sem_create(&s_item, 5);

    task_create(&p1, produtor, "Blargh");
    task_create(&p2, produtor, "Blargh");
    task_create(&p3, produtor, "Blargh");
    task_create(&c1, consumidor, "Blargh");
    task_create(&c2, consumidor, "Blargh");

    task_join(&p1);
    task_join(&p2);
    task_join(&p3);
    task_join(&c1);
    task_join(&c2);

    sem_destroy(&s_buffer);
    sem_destroy(&s_vaga);
    sem_destroy(&s_item);

    task_exit(0);
    exit(0);
}
