#pragma once
#include <cstddef>

struct TypeBuffer{
	char* buffer;
	size_t size;
	size_t offset;
	size_t reset_size;
};

#include <PieceTable.hpp>

void insert_char(TypeBuffer* type_buffer, PieceTable* table, char c);
void rdelete_char(TypeBuffer* type_buffer, PieceTable* table, int num_chars);
void ldelete_char(TypeBuffer* type_buffer, PieceTable* table, int num_chars);
void lseek(TypeBuffer* type_buffer, PieceTable* table, int num_chars);
void rseek(TypeBuffer* type_buffer, PieceTable* table, int  num_chars);
void reset_type_buffer(TypeBuffer* type_buffer);
