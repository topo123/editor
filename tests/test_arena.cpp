#include <catch2/catch_all.hpp>
#include "BumpArena.hpp"

TEST_CASE("Arena test","[arena]")
{
	BumpArena* arena = init_bump_arena();
	char* backed_mem = arena->head->memory;

	REQUIRE(backed_mem == (char*)(arena->head + 1));
	free_arena(arena);
}


TEST_CASE("Allocation test", "[arena]")
{
	BumpArena* arena = init_bump_arena(128);

	char* str = (char*)allocate(arena, 8, 1);
	str[0] = 'c';
	str[1] = 'h';
	str[2] = 'a';
	str[3] = 'r';
	char* str2 = (char*)allocate(arena, 16, 1);

	REQUIRE(str[0] == 'c');
	REQUIRE(arena->head->offset == 24);
	REQUIRE(str2 == ((char*)(char*)(arena->head + 1) + 8));
	free_arena(arena);
}

TEST_CASE("Growing arena", "[arena]")
{
	BumpArena* arena = init_bump_arena(128);
	Chunk* init_chunk = arena->head;

	char* str = (char*)allocate(arena, 128, 1);
	char* str2 = (char*)allocate(arena, 128, 1);

	REQUIRE(arena->head != init_chunk);
	REQUIRE(arena->head->chunk_size == 256);
	free_arena(arena);
}

struct small{
	long h;
	long g;
};

TEST_CASE("Alignment test", "[arena]")
{
	BumpArena* arena = init_bump_arena(128, 8);

	small* smat = (small*)allocate(arena, sizeof(small), 8);

	REQUIRE((uintptr_t)smat % 8 == 0);
	free_arena(arena);
}
