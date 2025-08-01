#version 330 core 

out vec4 FragColor;
in vec2 tex_coords;

uniform sampler2D bitmap_atlas_texture;

void main()
{
	FragColor = texture(bitmap_atlas_texture, tex_coords) * vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
