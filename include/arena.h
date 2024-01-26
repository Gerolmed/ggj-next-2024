#ifndef ARENA_H
#define ARENA_H

#include "include/types.h"

#define MEMORY_PAGE_SIZE 2000000
#define MEMORY_PAGE_COUNT 64


struct MemoryPage
{
    u8* memory;
    u32 next;
    u32 size;
    u32 current;
};

struct MemoryPool 
{
    MemoryPage pages[MEMORY_PAGE_COUNT];
    u32 free_pages[MEMORY_PAGE_COUNT];
    u32 free_count;
};

struct Arena
{
    MemoryPool* pool;
    i32 first;
    i32 page;
    u32 size;

    // On end_tmp() arena get reset to these values
    i32 tmp_page;
    u32 tmp_size;
    u32 tmp_current;
};

void init_pool(MemoryPool* pool);
i32 get_page(MemoryPool* pool, u32 min_size);
void free_page(MemoryPool* pool, i32 page_id);

void init_arena(Arena* arena, MemoryPool* pool);
void* push_size(Arena* arena, u32 size);
void begin_tmp(Arena* arena);
void end_tmp(Arena* arena);
void dispose(Arena* arena);
void copy(Arena* arena, void* dst);

extern MemoryPool pool;

#endif
