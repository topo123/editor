#include <catch2/catch_all.hpp>
#include <BumpAllocator.hpp>

TEST_CASE("Arena test","[arena]")
{
	BumpArena* arena = init_bump_arena();
	char* backed_mem = arena->head->memory;

	REQUIRE(backed_mem == (char*)(arena->head + 1));
}


TEST_CASE("Allocation test", "[arena]")
{
	BumpArena* arena = init_bump_arena();

	char* str = (char*)allocate(arena, 8);
	str[0] = 'c';
	str[1] = 'h';
	str[2] = 'a';
	str[3] = 'r';
	char* str2 = (char*)allocate(arena, 16);

	REQUIRE(str[0] == 'c');
	REQUIRE(arena->head->offset == 24);
	REQUIRE(str2 == ((char*)(char*)(arena->head + 1) + 8));
}
