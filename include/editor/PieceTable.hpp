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

#include "TypeBuffer.hpp"

PieceTable* init_piece_table(const std::string file_name, TypeBuffer* type_buffer, size_t type_buffer_size, size_t init_num_pieces = 8);
void insert_text(PieceTable* table, char* buffer, size_t buffer_size);
void rdelete_text(PieceTable* table, size_t num_chars);
void ldelete_text(PieceTable* table, size_t num_chars);
void lseek(PieceTable* table, size_t offset);
void rseek(PieceTable* table, size_t offset);
void free_table(PieceTable* table);
void print_table(PieceTable* table);
