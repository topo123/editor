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

BumpArena* init_bump_arena(size_t chunk_size = 128);
void* allocate(BumpArena* arena, size_t alloc_size);
int free_arena(BumpArena* arena);
