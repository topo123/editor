#include <cstring>
#include "PieceTable.hpp"
#include <TypeBuffer.hpp>
#include <cassert>

void insert_char(TypeBuffer* type_buffer, PieceTable* table, char c)
{
	if(type_buffer->size == 0)
	{
		insert_text(table, type_buffer->buffer, type_buffer->reset_size);
		reset_type_buffer(type_buffer);
		return;
	}

	type_buffer->buffer[type_buffer->offset] = c;
	type_buffer->offset ++;
	type_buffer->size --;
}

void rdelete_char(TypeBuffer* type_buffer, PieceTable* table, int num_chars)
{
	if(type_buffer->offset + type_buffer->size + num_chars > type_buffer->reset_size)
	{
		int piece_delete_chars = num_chars - (type_buffer->reset_size - (type_buffer->offset + type_buffer->size));
		type_buffer->size += type_buffer->reset_size - (type_buffer->offset + type_buffer->size);
		rdelete_text(table, piece_delete_chars);
		return;
	}

	type_buffer->size += num_chars;
}

void ldelete_char(TypeBuffer* type_buffer, PieceTable* table, int num_chars)
{
	if((int)type_buffer->offset - num_chars < 0 && type_buffer->size < type_buffer->reset_size)
	{
		int piece_delete_chars = num_chars - type_buffer->offset;
		type_buffer->size += type_buffer->offset; 
		type_buffer->offset = 0;
		ldelete_text(table, piece_delete_chars);
		return;
	}
	else if(type_buffer->size == type_buffer->reset_size)
	{
		ldelete_text(table, num_chars);
	}
	else
	{
		type_buffer->size += num_chars;
		type_buffer->offset -= num_chars;
	}
}

void lseek(TypeBuffer* type_buffer, PieceTable* table, int num_chars)
{
	if((int)type_buffer->offset - num_chars < 0 && type_buffer->size < type_buffer->reset_size)
	{
		int piece_move_chars = num_chars - type_buffer->offset;
		size_t copy_buffer_size = type_buffer->offset + (type_buffer->reset_size - (type_buffer->offset + type_buffer->size));
		char copy_buffer[copy_buffer_size];
		std::memcpy(copy_buffer, type_buffer->buffer, type_buffer->offset);
		std::memcpy(copy_buffer + type_buffer->offset, type_buffer->buffer + type_buffer->offset + type_buffer->size, 
							type_buffer->reset_size - (type_buffer->offset + type_buffer->size));
		insert_text(table, copy_buffer, copy_buffer_size);
		reset_type_buffer(type_buffer);
		lseek(table, piece_move_chars + copy_buffer_size);
	}
	else if(type_buffer->size == type_buffer->reset_size)
	{
		lseek(table, num_chars);
	}
	else
	{
		std::memcpy(type_buffer->buffer + type_buffer->offset + type_buffer->size - num_chars, type_buffer->buffer + type_buffer->offset - num_chars, num_chars);
		type_buffer->offset -= num_chars; 
	}

}

void rseek(TypeBuffer* type_buffer, PieceTable* table, int num_chars)
{
	if(type_buffer->size == type_buffer->reset_size)
	{
		rseek(table, num_chars);
	}
	else if(type_buffer->offset + type_buffer->size + num_chars > type_buffer->reset_size)
	{
		int piece_move_chars = num_chars - (type_buffer->reset_size - (type_buffer->offset + type_buffer->size));
		size_t copy_buffer_size = type_buffer->offset + type_buffer->reset_size - (type_buffer->offset + type_buffer->size);
		char copy_buffer[copy_buffer_size];
		std::memcpy(copy_buffer, type_buffer->buffer, type_buffer->offset);
		std::memcpy(copy_buffer + type_buffer->offset, type_buffer->buffer + type_buffer->offset + type_buffer->size,type_buffer->reset_size - (type_buffer->offset + type_buffer->size)); 
		insert_text(table, copy_buffer, copy_buffer_size);
		reset_type_buffer(type_buffer);
		rseek(table, piece_move_chars);
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
