#include "BumpArena.hpp"
#include "Renderer.hpp"
#include "PieceTable.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <map>
#include <cstring>

#define CURSOR_WIDTH 10.0f
#define CURSOR_HEIGHT 20.0f

std::map<unsigned char, RenderChar*> renderchar_map;

std::string vector_format(float x, float y)
{
	return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}

void print_char_metrics(char c)
{
	RenderChar* char_metrics = renderchar_map[c];
	std::cout << c << " metrics \n";
	std::cout << "Size: " << vector_format(char_metrics->size.x, char_metrics->size.y) << "\n";
	std::cout << "Advance: " << std::to_string(char_metrics->advance) << "\n";
	std::cout << "Bearing: " << vector_format(char_metrics->bearing.x, char_metrics->bearing.y) << std::endl;
}

int compile_shaders(const std::string vertex_shader_path, const std::string frag_shader_path)
{
	std::ifstream reader(vertex_shader_path);

	if(!reader)
	{
		std::cout << "Failed to open the vertex shader" << std::endl;
		return -1;
	}

	std::string vert_shader = "";
	std::string frag_shader = "";
	char c = '\0';

	while(reader.get(c))
	{
		vert_shader += c;
	}

	reader.close();
	reader.open(frag_shader_path);
	if(!reader)
	{
		std::cout << "Failed to open the fragment shader" << std::endl;
		return -1;
	}

	while(reader.get(c))
	{
		frag_shader += c;
	}

	const char* vert_shader_copy = (const char*)&vert_shader[0];
	const char* frag_shader_copy = (const char*)&frag_shader[0];

	unsigned int v_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(v_shader, 1, &vert_shader_copy, nullptr);
	glCompileShader(v_shader);

	int success;
	char log[512];
	glGetShaderiv(v_shader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(v_shader, 512, nullptr, log);
		std::cout << "ERROR LOG:VERT_SHADER: " << log << std::endl;
		return -1;
	}

	unsigned int f_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(f_shader, 1, &frag_shader_copy, nullptr);
	glCompileShader(f_shader);
	glGetShaderiv(f_shader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(f_shader, 512, nullptr, log);
		std::cout << "ERROR LOG:FRAG_SHADER: " << log << std::endl;
		return -1;
	}

	unsigned int shader_program = glCreateProgram();
	glAttachShader(shader_program, v_shader);
	glAttachShader(shader_program, f_shader);
	glLinkProgram(shader_program);

	glDeleteShader(v_shader);
	glDeleteShader(f_shader);

	return shader_program;
}

void init_render_data(Renderer* render)
{
	int quad_shader_program = compile_shaders("src/shaders/quad-vert-shader.glsl", "src/shaders/quad-frag-shader.glsl");
	int char_shader_program = compile_shaders("src/shaders/char-vert-shader.glsl", "src/shaders/char-frag-shader.glsl");

	assert(quad_shader_program > 0);
	assert(char_shader_program > 0);

	render->quad_shader_program = quad_shader_program;
	render->char_shader_program = char_shader_program;
	render->renderchar_arena = init_bump_arena(128 * sizeof(RenderChar), 4);

	glUseProgram(render->quad_shader_program);

	glm::mat4 ortho_proj = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
	int ortho_proj_location = glGetUniformLocation(render->quad_shader_program, "ortho_projection");
	assert(ortho_proj_location != -1);
	glUniformMatrix4fv(ortho_proj_location, 1, GL_FALSE, glm::value_ptr(ortho_proj));

	glUseProgram(render->char_shader_program);

	int ortho_proj_location1 = glGetUniformLocation(render->char_shader_program, "ortho_projection");
	assert(ortho_proj_location1 != -1);
	glUniformMatrix4fv(ortho_proj_location1, 1, GL_FALSE, glm::value_ptr(ortho_proj));

	unsigned int bitmap_atlas_texture = create_bitmap_font_atlas_texture("fonts/AdwaitaMono-Regular.ttf", render->renderchar_arena, 24, 16, 8, 0, 128);
	render->bitmap_atlas_texture = bitmap_atlas_texture;

	float quad_VBO[] = {
		0.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f
	};

	unsigned int quad_EBO[] = {
		0, 1, 2,
		0, 3, 2
	};

	unsigned int EBO;
	unsigned int VBO;
	unsigned int char_VBO;
	unsigned int offset_VBO;

	glGenVertexArrays(1, &render->quad_VAO);
	glGenVertexArrays(1, &render->char_instance_VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &char_VBO);
	glGenBuffers(1, &offset_VBO);

	render->char_VBO = char_VBO;
	render->offset_VBO = offset_VBO;

	glBindVertexArray(render->quad_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_VBO), quad_VBO, GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_EBO), quad_EBO, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	glBindVertexArray(render->char_instance_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_VBO), quad_VBO, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_EBO), quad_EBO, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	
	glBindBuffer(GL_ARRAY_BUFFER, render->char_VBO);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);

	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);

	glBindBuffer(GL_ARRAY_BUFFER, render->offset_VBO);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribDivisor(2, 1);

	glBindVertexArray(0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void render_piecetable(Renderer* render, const PieceTable* table, TypeBuffer* type_buffer, const unsigned int line_spacing, const unsigned int margin_width)
{
	const std::vector<Piece*>& piece_table = table->piece_list;
	std::vector<glm::mat4> char_model_matrices;
	std::vector<glm::vec4> char_uv_coords;

	size_t curr_cursor_pos = 0;

	float x_pos = margin_width;
	float y_pos = line_spacing;

	float cursor_x_pos = 0;
	float cursor_y_pos = 0;

	float char_x_pos = margin_width;
	float char_y_pos = 0;

	Piece* curr_piece;

	glm::mat4 model = glm::mat4(1.0f);

	for(size_t i = 0; i < piece_table.size(); i ++)
	{
		curr_piece = piece_table[i];
		curr_cursor_pos = curr_piece->offset;


		for(char* j = curr_piece->buffer + curr_piece->offset; j < (curr_piece->buffer + curr_piece->offset + curr_piece->length); j ++)
		{
			if(*j == '\n')
			{
				y_pos += line_spacing;
				x_pos = margin_width;
				continue;
			}

			if(type_buffer->size < type_buffer->reset_size && i == table->piece_index && table->cursor_pos == curr_cursor_pos)
			{
				size_t copy_buffer_size = type_buffer->reset_size -  type_buffer->size;
				size_t copy_buffer_offset = 0;
				char copy_buffer[copy_buffer_size];
				

				std::memcpy(copy_buffer, type_buffer->buffer, type_buffer->offset);
				std::memcpy(copy_buffer + type_buffer->offset, type_buffer->buffer + type_buffer->offset + type_buffer->size, type_buffer->reset_size - (type_buffer->offset + type_buffer->size));

				for(char* k = copy_buffer; k < copy_buffer + copy_buffer_size; k ++)
				{
					RenderChar* char_info = renderchar_map[*k];

					char_x_pos = x_pos + char_info->bearing.x;
					if(char_x_pos >  800 - margin_width)
					{
						char_x_pos = margin_width;
						x_pos = margin_width;
						y_pos += line_spacing;
					}
					char_y_pos = y_pos - char_info->bearing.y;

					if(copy_buffer_offset == type_buffer->offset + type_buffer->size - type_buffer->size && 
						type_buffer->offset + type_buffer->size < type_buffer->reset_size)
					{
						cursor_x_pos = *j == ' '? char_x_pos - (CURSOR_WIDTH - (float)(char_info->advance >> 6))/2.0f: char_x_pos - (CURSOR_WIDTH - (float)char_info->size.x)/2.0f;
						cursor_y_pos = char_y_pos - (CURSOR_HEIGHT - (float)char_info->size.y);
					}

					model = glm::translate(model, glm::vec3(glm::vec2(char_x_pos, char_y_pos), 0.0f));
					model = glm::scale(model, glm::vec3(glm::vec2(char_info->size.x, char_info->size.y), 1.0f));

					char_model_matrices.push_back(model);
					char_uv_coords.push_back(char_info->uv_coords);
					model = glm::mat4(1.0f);

					x_pos += (char_info->advance >> 6);
					copy_buffer_offset ++;
				}
			}

			RenderChar* char_info = renderchar_map[*j];

			char_x_pos = x_pos + char_info->bearing.x;
			if(char_x_pos >  800 - margin_width)
			{
				char_x_pos = margin_width;
				x_pos = margin_width;
				y_pos += line_spacing;
			}
			char_y_pos = y_pos - char_info->bearing.y;

			if(type_buffer->offset + type_buffer->size == type_buffer->reset_size 
				&& i == table->piece_index && table->cursor_pos == curr_cursor_pos)
			{
				cursor_x_pos = *j == ' '? char_x_pos - (CURSOR_WIDTH - (float)(char_info->advance >> 6))/2.0f: char_x_pos - (CURSOR_WIDTH - (float)char_info->size.x)/2.0f;
				cursor_y_pos = char_y_pos - (CURSOR_HEIGHT - (float)char_info->size.y);
			}

			model = glm::translate(model, glm::vec3(glm::vec2(char_x_pos, char_y_pos), 0.0f));
			model = glm::scale(model, glm::vec3(glm::vec2(char_info->size.x, char_info->size.y), 1.0f));

			char_model_matrices.push_back(model);
			char_uv_coords.push_back(char_info->uv_coords);
			model = glm::mat4(1.0f);

			x_pos += (char_info->advance >> 6);

			curr_cursor_pos ++;
		}
	}


	render_quad(render, {CURSOR_WIDTH, CURSOR_HEIGHT}, {cursor_x_pos, cursor_y_pos});
	assert(char_model_matrices.size() > 0 && char_model_matrices.size() == char_uv_coords.size());

	glUseProgram(render->char_shader_program);

	glBindVertexArray(render->char_instance_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, render->char_VBO);
	glBufferData(GL_ARRAY_BUFFER, char_model_matrices.size() * sizeof(glm::mat4), char_model_matrices.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, render->offset_VBO);
	glBufferData(GL_ARRAY_BUFFER, char_uv_coords.size() * sizeof(glm::vec4), char_uv_coords.data(), GL_DYNAMIC_DRAW);

	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, char_uv_coords.size()); 
	glBindVertexArray(0);
}

void render_character(Renderer* render, glm::vec2 position, unsigned char character)
{	
	glUseProgram(render->quad_shader_program);

	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, glm::vec3(position, 0.0f));
	model = glm::scale(model, glm::vec3(renderchar_map[character]->size, 1.0f));

	int model_ptr = glGetUniformLocation(render->quad_shader_program, "model");
	assert(model_ptr != -1);
	glUniformMatrix4fv(model_ptr, 1, GL_FALSE, glm::value_ptr(model));

	int shader_UV = glGetUniformLocation(render->quad_shader_program, "offset");
	assert(shader_UV != -1);
	glm::fvec4 uv_coords = renderchar_map[character]->uv_coords;
	glUniform4f(shader_UV, uv_coords.x, uv_coords.y, uv_coords.z, uv_coords.w);

	glBindTexture(GL_TEXTURE_2D, render->bitmap_atlas_texture);
	glBindVertexArray(render->quad_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void render_bitmap_atlas(Renderer *render, glm::vec2 size, glm::vec2 position)
{
	glUseProgram(render->quad_shader_program);

	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, glm::vec3(position, 0.0f));
	model = glm::scale(model, glm::vec3(size, 1.0f));

	int use_tex_ptr = glGetUniformLocation(render->quad_shader_program, "use_texture");
	assert(use_tex_ptr != -1);
	glUniform1i(use_tex_ptr, true);

	int model_ptr = glGetUniformLocation(render->quad_shader_program, "model");
	assert(model_ptr != -1);
	glUniformMatrix4fv(model_ptr, 1, GL_FALSE, glm::value_ptr(model));

	int shader_UV = glGetUniformLocation(render->quad_shader_program, "offset");
	assert(shader_UV != -1);
	glUniform4f(shader_UV, 0.0f, 0.0f, 1.0f, 1.0f);

	glBindTexture(GL_TEXTURE_2D, render->bitmap_atlas_texture);
	glBindVertexArray(render->quad_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

}

void render_quad(Renderer* render, glm::vec2 size, glm::vec2 position)
{
	glUseProgram(render->quad_shader_program);

	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, glm::vec3(position, 0.0f));
	model = glm::scale(model, glm::vec3(size, 1.0f));

	int use_tex_ptr = glGetUniformLocation(render->quad_shader_program, "use_texture");
	assert(use_tex_ptr != -1);
	glUniform1i(use_tex_ptr, false);

	int model_ptr = glGetUniformLocation(render->quad_shader_program, "model");
	assert(model_ptr != -1);
	glUniformMatrix4fv(model_ptr, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(render->quad_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

int create_bitmap_font_atlas_texture(const std::string font_path, BumpArena* renderchar_arena, const unsigned int text_height, const unsigned int horz_bitmap_cells, const unsigned int vert_bitmap_cells, unsigned char starting_character, unsigned char ending_character)
{
	FT_Library glyph_maker;
	if(FT_Init_FreeType(&glyph_maker))
	{
		std::cout << "Failed to initalize the free type library returning an error\n";
		return -1;
	}

	FT_Face glyph_face;
	if(FT_New_Face(glyph_maker, &font_path[0], 0, &glyph_face))
	{
		std::cout << "Failed to load a font returning an error\n";
		return -1;
	}

	FT_Set_Pixel_Sizes(glyph_face, 0, text_height);

	BumpArena* renderchar_data_arena = init_bump_arena(1024 * 1024, 1);
	std::map<unsigned char, unsigned char*> renderchar_data;
	unsigned int cell_width = 0;
	unsigned int cell_height = 0;

	for(unsigned char c = starting_character; c < ending_character; c ++)
	{

		if(FT_Load_Char(glyph_face, c, FT_LOAD_RENDER))
		{
			std::cout << "Failed to load " << c << " moving on.\n";
			return -1;
		}


		RenderChar* render_character = (RenderChar*)bump_alloc(renderchar_arena, sizeof(RenderChar), 4);
		render_character->advance = glyph_face->glyph->advance.x;
		render_character->bearing = glm::ivec2(glyph_face->glyph->bitmap_left, glyph_face->glyph->bitmap_top);
		render_character->size = glm::ivec2(glyph_face->glyph->bitmap.pitch, glyph_face->glyph->bitmap.rows);

		unsigned char* render_character_data = (unsigned char*)bump_alloc(renderchar_data_arena, render_character->size.x * render_character->size.y, 1);
		std::memcpy(render_character_data, glyph_face->glyph->bitmap.buffer, render_character->size.x * render_character->size.y);

		renderchar_data.insert(std::pair<char, unsigned char*>(c, render_character_data));
		renderchar_map.insert(std::pair<char, RenderChar*>(c, render_character));

		cell_width = cell_width < render_character->size.x? render_character->size.x: cell_width;
		cell_height = cell_height < render_character->size.y? render_character->size.y: cell_height;
	}

	int bitmap_width = cell_width * horz_bitmap_cells;
	int bitmap_height = cell_height * vert_bitmap_cells;

	unsigned char* bitmap_atlas = (unsigned char*)bump_alloc(renderchar_data_arena, bitmap_width * bitmap_height, 1);
	std::memset(bitmap_atlas, 1, bitmap_width * bitmap_height);

	int cell_row_number = 0;
	int cell_column_number = 0;

	int h_offset = 0;
	int v_offset = 0;


	unsigned char* render_char_buffer = nullptr;

	for(unsigned char c = starting_character; c < ending_character; c ++)
	{
		RenderChar* render_char = renderchar_map[c];

		h_offset = (cell_width - render_char->size.x)/2;
		v_offset = (cell_height - render_char->size.y)/2;

		render_char_buffer = renderchar_data[c];

		uintptr_t starting_px = (uintptr_t(bitmap_atlas) + 
			(bitmap_width * cell_height * cell_row_number + cell_column_number * cell_width)
			+ (v_offset * bitmap_width + h_offset));

		render_char->uv_coords = glm::fvec4((float)cell_column_number/(float)horz_bitmap_cells, 
																			((float)cell_row_number/(float)vert_bitmap_cells),
																			((float)cell_column_number/(float)horz_bitmap_cells) + (1.0f/(float)horz_bitmap_cells),
																			(((float)cell_row_number/(float)vert_bitmap_cells)) + (1.0f/(float)vert_bitmap_cells));

		for(int i = 0; i < render_char->size.y; i ++)
		{
			void* bitmap_atlas_position = (void*)(starting_px + (i * bitmap_width));
			void* render_char_buffer_copy_position = (void*)((uintptr_t)render_char_buffer + (i * render_char->size.x));
			assert((void*)render_char_buffer <= render_char_buffer_copy_position);
			std::memcpy(bitmap_atlas_position, render_char_buffer_copy_position, render_char->size.x);	
		}

		cell_column_number ++;
		if(cell_column_number == horz_bitmap_cells)
		{
			cell_column_number = 0;
			cell_row_number ++;
		}

	}


	unsigned int bitmap_atlas_texture;  
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &bitmap_atlas_texture);
	glBindTexture(GL_TEXTURE_2D, bitmap_atlas_texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, bitmap_width, bitmap_height, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap_atlas);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	free_arena(renderchar_data_arena);

	FT_Done_Face(glyph_face);
	FT_Done_FreeType(glyph_maker);

	return bitmap_atlas_texture;
}
