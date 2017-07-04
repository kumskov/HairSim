#version 400 core

in vec2 texCoords;

out vec3 color;

uniform sampler2D modelTexture;

void main()
{
	color = texture(modelTexture, texCoords).rgb;
}