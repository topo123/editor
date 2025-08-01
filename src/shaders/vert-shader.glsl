#version 330 core 

layout (location = 0) in vec2 quad_vertices;
layout (location = 1) in vec2 in_tex_coords;

out vec2 tex_coords;

uniform mat4 model;
uniform mat4 ortho_projection;

void main()
{
	tex_coords = in_tex_coords;
	gl_Position = ortho_projection * model * vec4(quad_vertices.xy, 0.0f, 1.0f);
}
