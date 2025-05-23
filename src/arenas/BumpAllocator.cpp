#include <BumpAllocator.hpp>
#include <cstdlib>

BumpArena* init_bump_arena(size_t chunk_size)
{
	BumpArena* arena = new BumpArena();

	Chunk* curr_chunk = (Chunk*)malloc(chunk_size + sizeof(Chunk));
	curr_chunk->offset = 0;
	curr_chunk->chunk_size = chunk_size;
	curr_chunk->memory = (char*)(curr_chunk + 1);
	curr_chunk->next = arena->head;
	arena->head = curr_chunk;

	return arena;
}

void* allocate(BumpArena* arena, size_t alloc_size)
{
	if(arena->head->offset + alloc_size >= arena->head->chunk_size)
	{
		Chunk* new_chunk = (Chunk*)malloc(arena->head->chunk_size * 2 + sizeof(Chunk));
		new_chunk->offset = 0;
		new_chunk->chunk_size = arena->head->chunk_size * 2;
		new_chunk->memory = (char*)(new_chunk + 1);
		new_chunk->next = arena->head;
		arena->head = new_chunk;
	}

	Chunk* alloc_chunk = arena->head;

	void* alloc = alloc_chunk->memory + alloc_chunk->offset;
	alloc_chunk->offset += alloc_size;

	return alloc;
}

int free_arena(BumpArena* arena)
{
	Chunk* chunk = arena->head;

	while(chunk)
	{
		free(chunk);
		chunk = chunk->next;
	}

	delete arena;

	return 0;
}
