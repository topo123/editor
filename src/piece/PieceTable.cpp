#include "PieceTable.hpp"
#include "BumpArena.hpp"
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>

const size_t PIECE_ALIGN = 8;
const size_t DEFAULT_ALIGN = 8;

PieceTable *init_piece_table(const std::string file_name, size_t type_buffer_size, size_t init_num_pieces) {
  std::ifstream text_file(file_name);
  char c = '\0';
  std::string file = "";

  if (!text_file.is_open()) {
    std::cout << "Could not open file\n";
    return nullptr;
  }

  while (text_file.get(c)) {
    file += c;
  }

  PieceTable *table = new PieceTable();
  table->mem_pieces = init_bump_arena(sizeof(Piece) * init_num_pieces, PIECE_ALIGN);
  table->text = init_bump_arena(type_buffer_size + file.size(), DEFAULT_ALIGN);

  char *original_buffer = (char *)bump_alloc(table->text, file.size(), 1);
  std::memcpy(original_buffer, file.data(), file.size());

  Piece *org_piece = (Piece *)bump_alloc(table->mem_pieces, sizeof(Piece), PIECE_ALIGN);

  org_piece->length = file.size();
  org_piece->offset = 0;
  org_piece->buffer = original_buffer;
  org_piece->piece_type = ORIGINAL;

  table->piece_list.push_back(org_piece);
  table->piece_index = 0;
  table->cursor_pos = 0;
  table->abs_cursor_pos = 0;
  table->text_size = file.size();

  assert(table->text_size > 0);

  return table;
}

void rdelete_text(PieceTable* table, size_t num_chars) {

  if(table->cursor_pos > 0 && 
    table->cursor_pos + num_chars < table->piece_list[table->piece_index]->length + table->piece_list[table->piece_index]->offset)
  {
    Piece* old_piece = table->piece_list[table->piece_index];
    Piece* new_piece = (Piece*)bump_alloc(table->mem_pieces, sizeof(Piece), PIECE_ALIGN);

    size_t old_length = old_piece->length;
    old_piece->length = table->cursor_pos - old_piece->offset;

    table->piece_list.insert(table->piece_list.begin() + table->piece_index + 1, new_piece);

    new_piece->buffer = old_piece->buffer;
    new_piece->offset = table->cursor_pos + num_chars;
    new_piece->piece_type = old_piece->piece_type;
    new_piece->length = old_length - num_chars - old_piece->length; 

    table->piece_index ++;
    table->cursor_pos = new_piece->offset;
    return;
  }

  if(table->cursor_pos > 0 && 
    table->cursor_pos + num_chars >= table->piece_list[table->piece_index]->length + table->piece_list[table->piece_index]->offset)
  {
    Piece* piece = table->piece_list[table->piece_index];
    piece->length -= piece->length + piece->offset - table->cursor_pos;
    num_chars -= piece->length + piece->offset - table->cursor_pos;
    
    if(table->piece_index == table->piece_list.size() - 1)
    {
      table->cursor_pos = piece->offset + piece->length - 1;
      return;
    }
    else
    {
      table->piece_index ++;
      table->cursor_pos = table->piece_list[table->piece_index]->offset;
    }
  }

  //hello
  //offset 1
  //4 chars delete

  while (num_chars > 0) {
    Piece *piece = table->piece_list[table->piece_index];

    if (table->piece_index == table->piece_list.size() - 1 && table->cursor_pos + num_chars >= piece->length + piece->offset) {
      piece->length = 0;
      break;
    }

    if (table->cursor_pos + num_chars >= piece->length + piece->offset) {
      num_chars -= piece->length - table->cursor_pos;
      piece->length = 0;
      table->piece_index ++;
      table->cursor_pos = table->piece_list[table->piece_index]->offset;
      table->abs_cursor_pos += piece->length;
    } else {
      table->cursor_pos += num_chars;
      table->abs_cursor_pos += num_chars;
      piece->offset += num_chars;
      piece->length -= num_chars;
      num_chars = 0;
    }
  }
}

void rseek(PieceTable *table, size_t offset) {
  while (offset > 0) {
    Piece *piece = table->piece_list[table->piece_index];

    if (table->piece_index == table->piece_list.size() - 1 && table->cursor_pos + offset >= piece->length + piece->offset) {
      table->cursor_pos = piece->offset + piece->length - 1;
      table->abs_cursor_pos += piece->length;
      break;
    }

    if (table->cursor_pos + offset >= piece->length + piece->offset) {
      offset -= piece->length - table->cursor_pos;
      table->piece_index++;
      table->cursor_pos = table->piece_list[table->piece_index]->offset;
      table->abs_cursor_pos += piece->length;
    } else {
      table->cursor_pos += offset;
      table->abs_cursor_pos += offset;
      offset = 0;
    }
  }
}

void ldelete_text(PieceTable* table, size_t num_chars) {

  if(table->cursor_pos < table->piece_list[table->piece_index]->length + table->piece_list[table->piece_index]->offset - 1 && 
    (int)table->cursor_pos - ((int)num_chars + (int)table->piece_list[table->piece_index]->offset) + 1 > 0) {

    Piece* old_piece = table->piece_list[table->piece_index];
    Piece* new_piece = (Piece*)bump_alloc(table->mem_pieces, sizeof(Piece), PIECE_ALIGN);

    size_t old_length = old_piece->length;
    old_piece->length = table->cursor_pos - num_chars - old_piece->offset + 1;

    table->piece_list.insert(table->piece_list.begin() + table->piece_index + 1, new_piece);

    new_piece->buffer = old_piece->buffer;
    new_piece->offset = table->cursor_pos + 1;
    new_piece->piece_type = old_piece->piece_type;
    new_piece->length = old_length - num_chars - old_piece->length; 

    table->piece_index ++;
    table->cursor_pos = new_piece->offset;
    return;
  }

  if(table->cursor_pos < table->piece_list[table->piece_index]->length + table->piece_list[table->piece_index]->offset - 1 && 
    (int)table->cursor_pos - ((int)num_chars + (int)table->piece_list[table->piece_index]->offset) + 1 <= 0) 
  {
    Piece* piece = table->piece_list[table->piece_index];
    size_t deleted_chars = table->cursor_pos - piece->offset + 1;
    piece->length -= table->cursor_pos - piece->offset + 1;  
    piece->offset = table->cursor_pos + 1;

    if(table->cursor_pos == 0)
    {
      table->cursor_pos = piece->offset;
      return;
    }
    else
    {
      table->piece_index --;
      table->cursor_pos = table->piece_list[table->piece_index]->offset + table->piece_list[table->piece_index]->length - 1;
    }
    num_chars -= deleted_chars;
  }


  while (num_chars > 0) {
    Piece *piece = table->piece_list[table->piece_index];

    if (table->piece_index == 0 && (int)table->cursor_pos - ((int)piece->offset + (int)num_chars) < 0) {
      table->cursor_pos = 0;
      piece->length = 0;
      break;
    }

    if ((int)table->cursor_pos - ((int)piece->offset + (int)num_chars) < 0) {
      num_chars -= table->cursor_pos - piece->offset + 1;
      piece->length = 0;
      table->piece_index --;
      table->cursor_pos = table->piece_list[table->piece_index]->length - 1;
      table->abs_cursor_pos -= (table->cursor_pos - piece->offset + 1);
    } else {
      piece->length -= num_chars + piece->offset;
      table->cursor_pos = piece->offset;
      table->abs_cursor_pos -= num_chars;
      num_chars = 0;
    }
  }
}

void lseek(PieceTable *table, size_t offset) {
  while (offset > 0) {
    Piece *piece = table->piece_list[table->piece_index];

    if (table->piece_index == 0 && (int)table->cursor_pos - ((int)offset + (int)piece->offset) < 0) {
      table->cursor_pos = 0;
      table->abs_cursor_pos = 0;
      break;
    }

    if ((int)table->cursor_pos - ((int)offset + (int)piece->offset) < 0) {
      offset -= (table->cursor_pos - piece->offset + 1);
      table->piece_index--;
      table->cursor_pos = table->piece_list[table->piece_index]->length - 1;
      table->abs_cursor_pos -= (table->cursor_pos - piece->offset + 1);
    } else {
      table->cursor_pos -= offset;
      table->abs_cursor_pos -= offset;
      offset = 0;
    }
  }
}

void insert_text(PieceTable* table, char* buffer, size_t buffer_size) {
  char* new_text = (char*)bump_alloc(table->text, buffer_size, 1);
  std::memcpy(new_text, buffer, buffer_size);

  if (table->cursor_pos == table->piece_list[table->piece_index]->length - 1) {
    Piece* piece = (Piece*)bump_alloc(table->mem_pieces, sizeof(Piece), PIECE_ALIGN);

    piece->buffer = new_text;
    piece->length = buffer_size;
    piece->offset = 0;
    piece->piece_type = EDIT;

    table->piece_list.insert(table->piece_list.begin() + table->piece_index + 1, piece);
    table->piece_index ++;
    table->cursor_pos = piece->length - 1;
    return;
  } else if (table->piece_list[table->piece_index]->length == 0) {
    Piece* piece = (Piece*)bump_alloc(table->mem_pieces, sizeof(Piece), PIECE_ALIGN);

    piece->buffer = new_text;
    piece->length = buffer_size;
    piece->offset = 0;
    piece->piece_type = EDIT;

    table->piece_list.insert(table->piece_list.begin() + table->piece_index + 1, piece);
    return;
  }

  Piece* former_piece = table->piece_list[table->piece_index];
  size_t old_length = former_piece->length;
  former_piece->length = table->cursor_pos - former_piece->offset + 1;

  Piece* new_text_piece = (Piece*)bump_alloc(table->mem_pieces, sizeof(Piece), PIECE_ALIGN);
  Piece* latter_split = (Piece*)bump_alloc(table->mem_pieces, sizeof(Piece), PIECE_ALIGN);

  table->piece_list.insert(table->piece_list.begin() + table->piece_index + 1, new_text_piece);
  table->piece_list.insert(table->piece_list.begin() + table->piece_index + 2, latter_split);

  new_text_piece->buffer = new_text;
  new_text_piece->offset = 0;
  new_text_piece->length = buffer_size;
  new_text_piece->piece_type = EDIT;

  latter_split->buffer = former_piece->buffer;
  latter_split->offset = table->cursor_pos + 1;
  latter_split->piece_type = former_piece->piece_type;
  latter_split->length = old_length - latter_split->offset;

  table->cursor_pos = new_text_piece->length - 1;
  table->abs_cursor_pos += buffer_size;
  table->text_size += buffer_size;
  table->piece_index ++;
}

void free_table(PieceTable* table)
{
  free_arena(table->text);
  free_arena(table->mem_pieces);
  table->piece_list.clear();
  delete table;
}

void print_table(PieceTable* table)
{
  for(Piece* piece: table->piece_list)
  {
    std::string str(piece->buffer + piece->offset, piece->length);
    std::cout << str;
  }
  std::cout << std::endl;
}
