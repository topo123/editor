#version 330 core 

out vec4 FragColor;
in vec2 tex_coords;

uniform sampler2D bitmap_atlas_texture;

void main()
{
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(bitmap_atlas_texture, tex_coords).r);
	FragColor = vec4(0, 0, 0, 1.0) * sampled;
}
