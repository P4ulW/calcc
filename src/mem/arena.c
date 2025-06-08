#include "arena.h"
void arena_init(Arena *arena, u32 size_in_bytes)
{
    arena->base_memory = malloc(size_in_bytes);
    if (NULL == arena->base_memory) {
        printf("Failed to alloc arena in file __FILE__");
    }
    arena->alloc_pos = arena->base_memory;
    arena->capacity  = size_in_bytes;
    arena->size      = 0;
    return;
}

void *arena_alloc(Arena *arena, u32 size_in_bytes)
{
    if (size_in_bytes > (arena->capacity - arena->size)) {
        printf(
            "\x1b[31mArena of size %d and capacity %d ",
            arena->size,
            arena->capacity);
        printf("has no space to alloc %d bytes\x1b[0m\n", size_in_bytes);
        exit(-1);
    }
    void *ptr        = arena->alloc_pos;
    arena->alloc_pos = (char *)arena->alloc_pos + size_in_bytes;
    arena->size += size_in_bytes;
    return ptr;
}

void arena_free(Arena *arena)
{
    free(arena->base_memory);
    arena->capacity  = 0;
    arena->size      = 0;
    arena->alloc_pos = NULL;
    return;
}
