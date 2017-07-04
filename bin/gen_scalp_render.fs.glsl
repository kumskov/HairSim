#version 400 core

/*
in GS_OUT 
{
	vec3 normal;
	vec3 color;
} gs_out; 
*/

in vec2 gs_texCoords;

out vec3 fs_color;

uniform sampler2D modelTexture;

void main()
{
	fs_color = texture(modelTexture, gs_texCoords).rgb;
}