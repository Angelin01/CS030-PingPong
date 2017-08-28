#include <stdio.h>
#include "queue.h"

void queue_append(queue_t** queue, queue_t* elem) {
    queue_t* first;
    queue_t* last; /* Auxiliares */
    if(!queue) { /* Se o ponteiro para a fila não for valido... */
        printf("Erro: a fila nao existe\n");
        return;
    }
    if(!elem) { /* Se o elemento nao existir... */
        printf("Erro: o elemento nao existe\n");
        return;
    }
    if(elem->next || elem->prev) { /* Se o elemento apontar para outros elementos, esta em fila */
        printf("Erro: o elemento ja esta em outra fila\n");
        return;
    }

    if(!*queue) { /* Se a fila esta vazia, elemento sera o primeiro */
        *queue = elem;
        elem->next = elem;
        elem->prev = elem;
        return;
    }
    /* Senao, guardar o primeiro elemento da fila */
    first = *queue;
    last = first->prev; /* Pega o ultimo elemento da fila */
    last->next = elem; /* Coloca o elemento no final da fila */
    elem->prev = last; /* Seta o anterior do elemento, o antigo final da fila */
    elem->next = first; /* Final da fila next aponta pro comeco */
    first->prev = elem; /* Comeco da fila prev aponta para o ultimo */
    return;
}

queue_t* queue_remove(queue_t** queue, queue_t* elem) {
    queue_t* aux;
    queue_t* first;

    if(!queue) { /* Se o ponteiro para a fila não for valido... */
        printf("Erro: a fila nao existe\n");
        return (NULL);
    }
    if(!*queue) {
        printf("Erro: a fila esta vazia\n");
        return (NULL);
    }
    if(!elem) { /* Se o elemento nao existir... */
        printf("Erro: o elemento nao existe\n");
        return (NULL);
    }

    first = *queue;

    if(first->next == elem && elem == first) { /* Se o elemento for o unico da fila */
        first->next = NULL;
        first->prev = NULL;
        *queue = NULL; /* Fila fica vazia */
        return (first);
    }

    aux = first; /* Senao, a fila nao tem apenas um elemento */
    do { /* Enquanto nao chegar no final da fila... */
        if(aux == elem) { /* Se achar o elemento a remover */
            aux->prev->next = aux->next;
            aux->next->prev = aux->prev;
            if(aux == first) {
                *queue = aux->next;
            }
            aux->next = NULL;
            aux->prev = NULL;
            return(aux);
        }
        aux = aux->next; /* Nao achou, continua procurando */
    } while(aux != first);
    /* Se sair do while chegou ao final (comeco de novo) da fila sem achar o elemento */
    printf("Erro: o elemento nao pertence a fila\n");
    return(NULL);
}

int queue_size(queue_t* queue) {
    queue_t* first = queue;
    queue_t* aux = first;
    int count;

    if(!queue) { /* Se a fila estiver vazia... */
        return(0);
    }

    count = 1; /* Se nao estiver vazia, percorre ate achar o final enquanto incrementa */
    while(aux->next != first) {
        count++;
        aux = aux->next;
    }
    return(count);
}

void queue_print(char* name, queue_t* queue, void print_elem(void*)) {
    queue_t* aux;

    printf("%s[", name); /* Printa o texto inicial */

    if(queue) {
        aux = queue;
        print_elem(aux); /* Para que o primeiro nao tenha espaco */
        aux = aux->next;
        do { /* Printa cada elemento... */
            printf(" ");
            print_elem(aux);
            aux = aux->next;
        } while(aux != queue); /* ... ate acabar a fila */
    }
    printf("]\n");
}
