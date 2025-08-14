#include <cstring>
#include "PieceTable.hpp"
#include <TypeBuffer.hpp>

void insert_char(TypeBuffer* type_buffer, PieceTable* table, char c)
{
	if(type_buffer->size == 0)
	{
		insert_text(table, type_buffer->buffer, type_buffer->reset_size);
		reset_type_buffer(type_buffer);
		return;
	}

	type_buffer->buffer[type_buffer->offset] = c;
	if(type_buffer->buffer + type_buffer->offset == type_buffer->buffer + type_buffer->size - 1)
	{
		type_buffer->size --;
	}
	else
	{
		type_buffer->offset ++;
		type_buffer->size --;
	}
}

void rdelete_char(TypeBuffer* type_buffer, PieceTable* table,  size_t num_chars)
{
	if(type_buffer->offset + type_buffer->size + num_chars > type_buffer->reset_size)
	{
		int piece_delete_chars = num_chars - (type_buffer->reset_size - (type_buffer->offset + type_buffer->size) + 1);
		type_buffer->size += type_buffer->reset_size - (type_buffer->offset + type_buffer->size);
		rdelete_text(table, piece_delete_chars);
		return;
	}

	type_buffer->size += num_chars;
}

void ldlete_char(TypeBuffer* type_buffer, PieceTable* table, size_t num_chars)
{
	if(type_buffer->offset - num_chars - 1 < 0)
	{
		int piece_delete_chars = num_chars - type_buffer->offset;
		type_buffer->size += type_buffer->offset; 
		type_buffer->offset = 0;
		ldelete_text(table, piece_delete_chars);
		return;
	}

	type_buffer->size += num_chars;
	type_buffer->offset -= num_chars;
}

void lseek(TypeBuffer* type_buffer, PieceTable* table, size_t num_chars)
{
	if(type_buffer->offset - num_chars - 1 < 0 && type_buffer->size > 0)
	{
		int piece_move_chars = num_chars - type_buffer->offset;
		insert_text(table, type_buffer->buffer + type_buffer->offset, type_buffer->size);
		reset_type_buffer(type_buffer);
		lseek(table, piece_move_chars);
	}
	else if(type_buffer->size == type_buffer->reset_size)
	{
		lseek(table, num_chars);
	}
	else
	{
		std::memcpy(type_buffer->buffer + type_buffer->offset, type_buffer->buffer + type_buffer->offset - num_chars, num_chars);
		type_buffer->offset -= num_chars; 
	}

}

void rseek(TypeBuffer* type_buffer, PieceTable* table, size_t num_chars)
{
	if(type_buffer->offset + type_buffer->size + num_chars > type_buffer->reset_size)
	{
		int piece_move_chars = num_chars - (type_buffer->reset_size - (type_buffer->offset + type_buffer->size) + 1);
		insert_text(table, type_buffer->buffer + type_buffer->offset, type_buffer->size);
		reset_type_buffer(type_buffer);
		rseek(table, piece_move_chars);
	}
	else if(type_buffer->size == type_buffer->reset_size)
	{
		rseek(table, num_chars);
	}
	else
	{
		std::memcpy(type_buffer->buffer + type_buffer->offset, type_buffer->buffer + type_buffer->offset + type_buffer->size, num_chars);
		type_buffer->offset += num_chars;
	}
}

void reset_type_buffer(TypeBuffer* type_buffer)
{
	type_buffer->offset = 0;
	type_buffer->size = type_buffer->reset_size;
}
