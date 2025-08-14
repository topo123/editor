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
void rdelete_char(TypeBuffer* type_buffer, PieceTable* table, size_t num_chars);
void ldlete_char(TypeBuffer* type_buffer, PieceTable* table, size_t num_chars);
inline void lseek(TypeBuffer* type_buffer, PieceTable* table, size_t num_chars);
inline void rseek(TypeBuffer* type_buffer, PieceTable* table, size_t  num_chars);
inline void reset_type_buffer(TypeBuffer* type_buffer);
