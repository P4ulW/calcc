#ifndef ARENA_H
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef float f32;
typedef double f64;
typedef uint32_t u32;
typedef int32_t i32;

typedef struct Arena {
    void *base_memory;
    void *alloc_pos;
    u32 capacity;
    u32 size;
} Arena;

void arena_init(Arena *arena, u32 size_in_bytes);
void *arena_alloc(Arena *arena, u32 size_in_bytes);
void arena_free(Arena *arena);

#endif
