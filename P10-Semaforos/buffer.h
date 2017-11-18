#ifndef __BUFFER__
#define __BUFFER__

#define SIZEBUFFER 10

typedef struct buffer {
    int slot[SIZEBUFFER];
    int i;
} buffer;

void buffer_create(buffer* buff);

void buffer_insert(buffer* buff, int element);

int buffer_remove(buffer* buff);

#endif
