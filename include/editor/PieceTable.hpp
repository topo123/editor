#pragma once
#include <string>
#include <cstddef>
#include <vector>
#include <BumpArena.hpp>

enum Ptype{
	ORIGINAL,
	EDIT
};

struct Piece
{
	size_t length;
	size_t offset;
	char* buffer;
	Ptype piece_type;
};

struct PieceTable
{
	size_t piece_index;
	size_t cursor_pos;
	size_t abs_cursor_pos;
	size_t text_size;
	BumpArena* mem_pieces;
	BumpArena* text;
	std::vector<Piece*> piece_list;
};

PieceTable* init_piece_table(PieceTable* table, const std::string file_name, size_t type_buffer_size, size_t init_store_buffer_size);
void insert_text(PieceTable* table, char* buffer, size_t insert_pos);
void rdelete_text(PieceTable* table, size_t num_chars);
void ldelete_text(PieceTable* table, size_t num_chars);
void lseek(PieceTable* table, size_t offset);
void rseek(PieceTable* table, size_t offset);
