#include <catch2/catch_all.hpp>
#include "PieceTable.hpp"
#include <cstring>

void test_character(PieceTable* table, char c)
{
	Piece* piece = table->piece_list[table->piece_index];
	REQUIRE(*(piece->buffer + table->cursor_pos) == c);
}

void test_string(Piece* piece, std::string str)
{
	std::string str1(piece->buffer + piece->offset, piece->length);
	REQUIRE(str == str1);
}

void test_string_size(Piece* piece, size_t size)
{
	std::string str1(piece->buffer + piece->offset, piece->length);
	REQUIRE(size == str1.size());
}

TEST_CASE("PieceTable init test", "[piece]")
{
	PieceTable* table = init_piece_table("build-tests/test-files/hello.txt" , 256, 16);

	REQUIRE(table->piece_list.size() == 1);
	REQUIRE(table->piece_list[table->piece_index]->length == 35);
	REQUIRE(table->piece_index == 0);

	char str[35];
	std::memcpy(str, table->piece_list[table->piece_index]->buffer, 35);

	std::string copy_string(str, 35);
	REQUIRE(copy_string == "Hello my name is Olusola Awonusonu\n");
	REQUIRE(table->piece_list[table->piece_index]->offset == 0);

	free_table(table);
}


TEST_CASE("Piecetable move test", "[piece]")
{
	PieceTable* table = init_piece_table("build-tests/test-files/hello.txt" , 256, 16);
	Piece* curr_piece = table->piece_list[table->piece_index];

	rseek(table, 4);

	REQUIRE(*(curr_piece->buffer + table->cursor_pos) == 'o');
	REQUIRE(table->cursor_pos == table->abs_cursor_pos);
	REQUIRE(table->cursor_pos == 4);

	free_table(table);
}


TEST_CASE("Insert test", "[piece]")
{
	PieceTable* table = init_piece_table("build-tests/test-files/hello.txt" , 256, 16);
	char insert_buffer[] = "gold";
	char space[] = " ";
	//Hello my name is Olusola Awonusonu\n

	rseek(table, 5);
	test_character(table, ' ');
	//Hello my name is Olusola Awonusonu\n
	//     ^
	
	insert_text(table, insert_buffer, 4);

	test_character(table, 'd');
	test_string(table->piece_list[0], "Hello ");
	test_string(table->piece_list[1], "gold");
	test_string(table->piece_list[2], "my name is Olusola Awonusonu\n");

	REQUIRE(table->piece_list[2]->length == 29);
	REQUIRE(table->piece_list[2]->offset == 6);

	insert_text(table, space, 1);
	test_string(table->piece_list[table->piece_index], " ");

	free_table(table);
}

TEST_CASE("Move test", "[piece]")
{
	PieceTable* table = init_piece_table("build-tests/test-files/hello.txt" , 256, 16);
	char insert_buffer[] = "gold";
	char space[] = " ";
	//Hello my name is Olusola Awonusonu\n

	rseek(table, 5);
	test_character(table, ' ');
	//Hello my name is Olusola Awonusonu\n
	//     ^

	insert_text(table, insert_buffer, 4);

	test_character(table, 'd');
	test_string(table->piece_list[0], "Hello ");
	test_string(table->piece_list[1], "gold");
	test_string(table->piece_list[2], "my name is Olusola Awonusonu\n");

	REQUIRE(table->piece_list[2]->length == 29);
	REQUIRE(table->piece_list[2]->offset == 6);

	insert_text(table, space, 1);
	test_string(table->piece_list[table->piece_index], " ");

	lseek(table, 100);
	test_character(table, 'H');

	rseek(table, 100);
	REQUIRE(table->piece_index == table->piece_list.size() - 1);
	test_character(table, '\n');

	lseek(table, 30);
	test_character(table, 'd');

	free_table(table);
}

TEST_CASE("Delete test", "[piece]")
{
	PieceTable* table = init_piece_table("build-tests/test-files/hello.txt", 256, 16);
	rdelete_text(table, 6);

	std::string comp_string = "my name is Olusola Awonusonu\n";
	test_character(table, 'm');
	test_string(table->piece_list[0], comp_string);
	test_string_size(table->piece_list[0], comp_string.size());

	rseek(table, 7);
	test_character(table, ' ');
	ldelete_text(table, 7);

	comp_string = "is Olusola Awonusonu\n";
	test_string(table->piece_list[table->piece_list.size() - 1], comp_string);
	test_string_size(table->piece_list[table->piece_list.size() - 1], comp_string.size());

	print_table(table);
	free_table(table);

}

TEST_CASE("Left Delete across multiple pieces", "[piece]")
{
	PieceTable* table = init_piece_table("build-tests/test-files/hello.txt", 256, 16);
	char insert[] = " gold ";

	insert_text(table, insert, 6);
	REQUIRE(table->piece_index == 1);
	rseek(table, 1);

	ldelete_text(table, 4);
	test_string(table->piece_list[1], " go");
	test_string(table->piece_list[2], "llo my name is Olusola Awonusonu\n");

	free_table(table);
}

TEST_CASE("Left delete from end of piece", "[piece]")
{
	PieceTable* table = init_piece_table("build-tests/test-files/hello.txt", 256, 16);
	rseek(table, 100);
	ldelete_text(table, 11);

	test_string(table->piece_list[0], "Hello my name is Olusola");
	free_table(table);
}


TEST_CASE("Right delete middle of piece", "[piece]")
{
	//Hello my name is Olusola Awonusonu
	PieceTable* table = init_piece_table("build-tests/test-files/hello.txt", 256, 16);
	rseek(table, 5);

	rdelete_text(table, 9);

	test_string(table->piece_list[0], "Hello");
	test_string(table->piece_list.back(), "is Olusola Awonusonu\n");

	free_table(table);
}

TEST_CASE("Right delete to the end of piece", "[piece]")
{
	//Hello my name is Olusola Awonusonu
	PieceTable* table = init_piece_table("build-tests/test-files/hello.txt", 256, 16);
	rseek(table, 5);
	rdelete_text(table, 30);

	test_string(table->piece_list[0], "Hello");

	free_table(table);

	
}

TEST_CASE("Right delete across multiple pieces", "[piece]")
{
	//Hello my name is Olusola Awonusonu
	PieceTable* table = init_piece_table("build-tests/test-files/hello.txt", 256, 16);
	char insert[] = "gold ";

	rseek(table, 5);
	insert_text(table, insert, 5);

	REQUIRE(table->piece_index == 1);
	REQUIRE(table->cursor_pos == 4);

	lseek(table, 2);
	rdelete_text(table, 3 + 8);

	test_string(table->piece_list[1], "go");
	test_string(table->piece_list[2], "Olusola Awonusonu\n");

	free_table(table);
}
