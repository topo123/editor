#include <BumpArena.hpp>
#include <cstdlib>
#include <cstdint>


inline uintptr_t align_ptr(uintptr_t address, size_t align_size)
{
	uintptr_t addr = (uintptr_t)address;
	addr = (addr + align_size - 1) & ~(align_size - 1);
	return addr;
}

BumpArena* init_bump_arena(size_t chunk_size, size_t align_size)
{
	BumpArena* arena = new BumpArena();

	void* memory = malloc(chunk_size + (align_size - 1) + sizeof(Chunk));
	uintptr_t chunk_memory = align_ptr((uintptr_t)memory + sizeof(Chunk), align_size);
	Chunk* curr_chunk = (Chunk*)memory;

	curr_chunk->offset = 0;
	curr_chunk->chunk_size = chunk_size;
	curr_chunk->memory = (char*)chunk_memory;
	curr_chunk->next = arena->head;
	arena->head = curr_chunk;

	return arena;
}

void* bump_alloc(BumpArena* arena, size_t alloc_size, size_t align_size)
{
	if(arena->head->offset + alloc_size > arena->head->chunk_size)
	{
		void* memory = malloc(arena->head->chunk_size * 2 + sizeof(Chunk) + (align_size - 1));
		uintptr_t chunk_memory = align_ptr((uintptr_t)memory + sizeof(Chunk), align_size);
		Chunk* new_chunk = (Chunk*)memory;

		new_chunk->offset = 0;
		new_chunk->chunk_size = arena->head->chunk_size * 2;
		new_chunk->memory = (char*)chunk_memory;
		new_chunk->next = arena->head;

		arena->head = new_chunk;
	}

	Chunk* alloc_chunk = arena->head;

	void* alloc = (void*)align_ptr((uintptr_t)alloc_chunk->memory + alloc_chunk->offset, align_size);
	alloc_chunk->offset += alloc_size;

	return alloc;
}

int free_arena(BumpArena* arena)
{
	Chunk* chunk = arena->head;

	while(chunk)
	{
		Chunk* free_chunk = chunk;
		chunk = chunk->next;
		free(free_chunk);
	}

	delete arena;

	return 0;
}
