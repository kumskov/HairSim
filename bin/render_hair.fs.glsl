#version 400 core

in vec3 vs_color;

out vec3 color;

void main()
{
	color = vs_color;
	color = vec3(255, 0, 255);
}