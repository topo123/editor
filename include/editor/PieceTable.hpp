#pragma once
#include <cstddef>

class PieceTable
{
	struct Piece
	{
		enum PieceType{
			ORIGNAL,
			EDIT
		};
		size_t length;
		size_t offset;
		char* buffer;
	};


	size_t piece_string_size;
};
