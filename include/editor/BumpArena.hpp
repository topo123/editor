#pragma once
#include <cstddef>

struct Chunk{
	size_t chunk_size;
	size_t offset;
	Chunk* next;
	char* memory;
};

struct BumpArena{
	Chunk* head {nullptr};
};

inline void* align_ptr(void* address, size_t align_size);
BumpArena* init_bump_arena(size_t chunk_size = 128, size_t align_size = 16);
void* bump_alloc(BumpArena* arena, size_t alloc_size, size_t align_size);
int free_arena(BumpArena* arena);
