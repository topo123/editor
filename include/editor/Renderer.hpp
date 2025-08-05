#pragma once
#include <string>
#include <BumpArena.hpp>
#include <glm/glm.hpp>
#include <map>


struct RenderChar{
	glm::ivec2 size;
	glm::ivec2 bearing;
	glm::fvec4 uv_coords;
	unsigned int advance;
};

struct Renderer{
	BumpArena* renderchar_arena;
	unsigned int bitmap_atlas_texture;
	unsigned int quad_VAO;
	unsigned int shader_program;
};

extern std::map<unsigned char, RenderChar*> renderchar_map;

void render_character(Renderer* render, glm::vec2 position, unsigned char character);
void render_quad(Renderer* render, glm::vec2 size, glm::vec2 position);
int create_bitmap_font_atlas_texture(const std::string font_path, BumpArena* renderchar_arena, const unsigned int text_height, const unsigned int horz_bitmap_cells, const unsigned int vert_bitmap_cells, unsigned char starting_character, unsigned char ending_character);
int compile_shaders(const std::string vertex_shader_path, const std::string frag_shader_path);
void init_render_data(Renderer* render, const std::string v_shader_path, const std::string f_shader_path);
