#version 330 core 

out vec4 FragColor;
in vec2 tex_coords;

uniform sampler2D bitmap_atlas_texture;
uniform bool use_texture;

void main()
{
	if(use_texture)
	{
		FragColor = texture(bitmap_atlas_texture, tex_coords);
	}
	else
	{
		FragColor = vec4(1.0, 1.0, 1.0, 1.0);
	}
}
