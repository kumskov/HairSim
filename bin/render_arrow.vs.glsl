#version 400 core

layout(location = 0) in vec3 mdlVertexPos;

uniform mat4 mvpMatrix;

void main()
{
	vec4 position = vec4(mdlVertexPos, 1);

	gl_Position = mvpMatrix * position;
}