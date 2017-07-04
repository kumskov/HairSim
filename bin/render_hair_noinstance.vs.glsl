#version 400 core

layout(location = 0) in vec4 hairVertexPos;

uniform mat4 mvpMatrix;

out vec3 vs_color;

void main()
{
	vec4 fixedPosition = hairVertexPos;

	gl_Position = mvpMatrix * fixedPosition;
	vs_color = vec3(255, 0, 0);
}