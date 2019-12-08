#ifndef clox_memory_h
#define clox_memory_h

#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

#define GROW_ARRAY(previous, type, oldCount, count) \
    (type*)reallocate(previous, sizeof(type) * (oldCount), \
        sizeof(type) * (count))

#define ALLOCATE(type, count) \
    (type *)reallocate(NULL, 0, sizeof(type) * (count))

#define NEW(size) alloc(size)

void *reallocate(void *previous, size_t oldSize, size_t newSize);
void *alloc(size_t size);

#endif