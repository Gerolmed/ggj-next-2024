#include "include/arena.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <include/game_math.h>

// NOTE: I regret all of this :(
void init_pool(MemoryPool* pool)
{
    pool->free_count = MEMORY_PAGE_COUNT;
    for (u32 i = 0; i < MEMORY_PAGE_COUNT; ++i) {
        pool->pages[i] = {};
        pool->free_pages[i] = i;
    }
};

void init_arena(Arena* arena, MemoryPool* pool)
{
    arena->page = -1;
    arena->first = -1;
    arena->tmp_page = -1;
    arena->size = 0;
    arena->tmp_size = 0;
    arena->tmp_current = 0;
    pool->pages[arena->page].current = 0;
    arena->pool = pool;
}

void* push_size(Arena* arena, u32 size)
{
    if (arena->first < 0 || arena->page < 0) {
        assert(arena->first == arena->page);
        arena->first = get_page(arena->pool, size);
        arena->page = arena->first;
    }
    
    MemoryPage* p = arena->pool->pages + arena->page;
    if (p->size - p->current >= size) {
        void* result = p->memory + p->current;
        p->current += size;
        arena->size += size;
        return result;
    } else {
        i32 n_id = get_page(arena->pool, size);
        MemoryPage* n = arena->pool->pages + n_id;
        p->next = n_id;
        arena->page = n_id;
        n->current = size;
        arena->size += size;
        return n->memory;
    }
};

void begin_tmp(Arena* arena)
{
    arena->tmp_page = arena->page;
    arena->tmp_size = arena->size;
    arena->tmp_current = arena->pool->pages[arena->page].current;
}

void end_tmp(Arena* arena)
{
    if (arena->tmp_page == -1) {
        dispose(arena);
        return;
    } 

    i32 page_ptr = arena->pool->pages[arena->tmp_page].next;
    while (page_ptr >= 0) {
        free_page(arena->pool, page_ptr);
        if (page_ptr == arena->page) {
            break;
        }
        page_ptr = arena->pool->pages[page_ptr].next;
    }


    arena->page = arena->tmp_page;
    arena->size = arena->tmp_size;
    arena->pool->pages[arena->page].current = arena->tmp_current;
    arena->pool->pages[arena->page].next = -1;
    arena->tmp_page = -1;
}

void copy(Arena* arena, void* dst)
{
    u8* dest = (u8*) dst;
    i32 page_ptr = arena->first;
    u32 byte_offset = 0;
    while (page_ptr >= 0) {
        MemoryPage page = arena->pool->pages[page_ptr];
        memcpy(dest + byte_offset, page.memory, page.current);
        byte_offset += page.current;
        if (page_ptr == arena->page) break;
        page_ptr = page.next;
    }
}

i32 get_page(MemoryPool* pool, u32 min_size)
{
    // TODO: Handle this somehow
    assert(pool->free_count > 0);

    // Look if a suitable page that is already allocated
    for (u32 i = 0; i < pool->free_count; ++i) {
        u32 page_id = pool->free_pages[i];
        MemoryPage* page = pool->pages + page_id;
        if (page->memory && page->size > min_size) {
            pool->free_count--;
            pool->free_pages[i] = pool->free_pages[pool->free_count];

            pool->pages[page_id].next = -1;
            pool->pages[page_id].current = 0;

            return page_id;
        }
    }

    // Try to allocate a new page
    u32 size = max(min_size, MEMORY_PAGE_SIZE);
    for (u32 i = 0; i < pool->free_count; ++i) {
        u32 page_id = pool->free_pages[i];
        MemoryPage* page = pool->pages + page_id;
        if (page->memory == NULL) {
            pool->free_count--;
            pool->free_pages[i] = pool->free_pages[pool->free_count];

            page->memory = (u8*) malloc(size);
            page->size = size;
            pool->pages[page_id].next = -1;
            pool->pages[page_id].current = 0;

            return page_id;
        }
    }

    // TODO: Try realloc unused but already allocated page
    assert(0);
    return -1;
}

void free_page(MemoryPool* pool, i32 page_id)
{
    pool->free_pages[pool->free_count] = page_id;
    pool->free_count++;
}

void dispose(Arena* arena)
{
    i32 page = arena->first;
    while (page >= 0) {
        free_page(arena->pool, page);
        page = arena->pool->pages[page].next;
    }
    arena->first = -1;
    arena->page = -1;
    arena->size = 0;
    arena->tmp_size = 0;
    arena->tmp_page = -1;
    arena->tmp_current = 0;
}

MemoryPool pool;
