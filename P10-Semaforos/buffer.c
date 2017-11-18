#include "buffer.h"

void buffer_create(buffer* buff) {
    buff->i = 0;
}

void buffer_insert(buffer* buff, int element) {
    buff->slot[buff->i++] = element;
}

int buffer_remove(buffer* buff) {
    int i;
    int toRemove = buff->slot[0];

    for(i = 0; i < buff->i; ++i) {
        buff->slot[i-1] = buff->slot[i];
    }
    --buff->i;

    return (toRemove);
}
