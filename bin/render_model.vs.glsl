#version 400 core

layout(location = 0) in vec3 mdlVertexPos;
layout(location = 1) in vec2 textureCoords;

out vec2 texCoords;

uniform mat4 mvpMatrix;

void main()
{
	texCoords = textureCoords;

	vec4 position = vec4(mdlVertexPos, 1);

	gl_Position = mvpMatrix * position;
}