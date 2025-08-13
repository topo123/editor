#version 330 core 

layout (location = 0) in vec2 quad_vertices;
layout (location = 1) in vec2 in_tex_coords;
layout (location = 2) in vec4 in_uv_coords;
layout (location = 3) in mat4 char_model_matrix;

out vec2 tex_coords;

uniform mat4 ortho_projection;

void main()
{
	tex_coords = mix(in_uv_coords.xy, in_uv_coords.zw, in_tex_coords);
	gl_Position = ortho_projection * char_model_matrix * vec4(quad_vertices.xy, 0.0f, 1.0f);
}
