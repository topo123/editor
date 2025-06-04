#include "BumpArena.hpp"
#include "PieceTable.hpp"
#include <cassert>
#include <fstream>
#include <cstring>
#include <iostream>

const size_t PIECE_ALIGN = 8;

PieceTable* init_piece_table(const std::string file_name, size_t type_buffer_size, size_t init_store_buffer_size, size_t init_num_pieces)
{

	std::ifstream text_file(file_name);
	char c = '\0';
	std::string file = "";

	if(!text_file.is_open())
	{
		std::cout << "Could not open file\n";
		return nullptr;
	}

	while(text_file.get(c))
	{
		file += c;
	}
 
	PieceTable* table = new PieceTable();
	table->mem_pieces = init_bump_arena(sizeof(Piece) * init_num_pieces, 8);
	table->text = init_bump_arena(type_buffer_size + init_store_buffer_size + file.size(), 1);
	table->cursor_pos = 0;

	char* original_buffer = (char*)bump_alloc(table->text, file.size(), 1);
	std::memcpy(original_buffer,file.data(), file.size());

	Piece* org_piece = (Piece*)bump_alloc(table->mem_pieces, sizeof(Piece),  PIECE_ALIGN);

	org_piece->length = file.size();
	org_piece->offset = 0;
	org_piece->buffer = original_buffer;
	org_piece->piece_type = ORIGINAL;

	table->piece_list.push_back(org_piece);
	table->piece_index = 0;
	table->cursor_pos = 0;
	table->abs_cursor_pos = 0;
	table->text_size = file.size();

	return table;
}


void rseek(PieceTable* table, size_t offset)
{
	while(offset > 0)
	{
		Piece* piece = table->piece_list[table->piece_index];

		if(table->piece_index == table->piece_list.size() - 1 && table->cursor_pos + offset >= piece->length)
		{
			table->cursor_pos = piece->length - 1;
			table->abs_cursor_pos += piece->length - table->cursor_pos - 1;
			break;
		}

		if(table->cursor_pos + offset >= piece->length)
		{
			offset -= piece->length - table->cursor_pos;
			table->piece_index ++;
			table->cursor_pos = table->piece_list[table->piece_index]->offset;
			table->abs_cursor_pos += offset;
		}
		else {
			table->cursor_pos += offset;
			table->abs_cursor_pos += offset;
			offset = 0;
		}
	}
}


void lseek(PieceTable* table, size_t offset)
{
	while(offset > 0)
	{
		Piece* piece = table->piece_list[table->piece_index];

		if(table->piece_index == 0 && table->cursor_pos - offset < 0)
		{
			table->cursor_pos = 0;
			table->abs_cursor_pos = 0; 
			break;
		}

		if(table->cursor_pos - offset < 0)
		{
			offset -= table->cursor_pos;
			table->piece_index --;
			table->cursor_pos = table->piece_list[table->piece_index]->length - 1;
			table->abs_cursor_pos -=  table->cursor_pos;
		}
		else {
			table->cursor_pos -= offset;
			table->abs_cursor_pos -= offset;
			offset = 0;
		}
	}
}


void insert_text(PieceTable* table, char* buffer, size_t buffer_size)
{
	char* new_text = (char*)bump_alloc(table->text, buffer_size, 1);
	std::memcpy(buffer, new_text, buffer_size);

	Piece* piece = nullptr; 

	if(table->cursor_pos == 0)
	{
		piece = (Piece*)bump_alloc(table->mem_pieces, sizeof(Piece), PIECE_ALIGN);

		piece->buffer = new_text;
		piece->length = buffer_size;
		piece->offset = 0; 
		piece->piece_type = EDIT;

		table->piece_list.insert(table->piece_list.begin() + table->piece_index, piece);	
		return;
	}
	else if(table->cursor_pos == table->piece_list[table->piece_index]->length - 1)
	{
		piece = (Piece*)bump_alloc(table->mem_pieces, sizeof(Piece), PIECE_ALIGN);

		piece->buffer = new_text;
		piece->length = buffer_size;
		piece->offset = 0;
		piece->piece_type = EDIT;

		table->piece_list.insert(table->piece_list.begin() + table->piece_index + 1, piece);
		return;
	}
	else if(table->piece_list[table->piece_index]->length == 0)
	{	
		piece = (Piece*)bump_alloc(table->mem_pieces, sizeof(Piece), PIECE_ALIGN);

		piece->buffer = new_text;
		piece->length = buffer_size;
		piece->offset = 0;
		piece->piece_type = EDIT;

		table->piece_list.insert(table->piece_list.begin() + table->piece_index + 1, piece);
		return;

	}

	size_t old_length = table->piece_list[table->piece_index]->length;
	table->piece_list[table->piece_index]->length = table->cursor_pos - table->piece_list[table->piece_index]->offset;

	Piece* new_text_piece = (Piece*)bump_alloc(table->mem_pieces, sizeof(Piece), PIECE_ALIGN);
	Piece* latter_split = (Piece*)bump_alloc(table->mem_pieces, sizeof(Piece), PIECE_ALIGN);

	table->piece_index ++;	

	table->piece_list.insert(table->piece_list.begin() + table->piece_index, new_text_piece);
	table->piece_list.insert(table->piece_list.begin() + table->piece_index + 1, latter_split);
	
	new_text_piece->buffer = new_text;
	new_text_piece->offset = 0;
	new_text_piece->length = buffer_size;
	new_text_piece->piece_type = EDIT;

	latter_split->buffer = new_text_piece->buffer;
	latter_split->offset = table->cursor_pos + 1;
	latter_split->piece_type = table->piece_list[table->piece_index - 1]->piece_type;
	latter_split->length = old_length - table->cursor_pos;

	table->cursor_pos = new_text_piece->length - 1;
	table->text_size += buffer_size;
}

void rdelete_text(PieceTable* table, size_t num_chars)
{

	while(num_chars > 0)
	{
		Piece* piece = table->piece_list[table->piece_index];

		if(table->piece_index == table->piece_list.size() - 1 && table->cursor_pos + num_chars >= piece->length)
		{
			table->cursor_pos = piece->length - 1;
			table->abs_cursor_pos += piece->length - table->cursor_pos - 1;
			break;
		}

		if(table->cursor_pos + num_chars >= piece->length)
		{
			num_chars -= piece->length - table->cursor_pos;
			table->piece_index ++;
			table->cursor_pos = table->piece_list[table->piece_index]->offset;
			table->abs_cursor_pos += num_chars;
		}
		else {
			table->cursor_pos += num_chars;
			table->abs_cursor_pos += num_chars;
			num_chars = 0;
		}
	}

}

void ldelete_text(PieceTable* table, size_t num_chars)
{
}
