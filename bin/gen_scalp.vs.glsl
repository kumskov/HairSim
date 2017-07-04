#version 400 core

layout(location = 0) in vec3 mdlVertexPos;
layout(location = 1) in vec2 textureCoords;

out VS_OUT
{
	vec3 color;
	bool skip;
	vec3 position;
} vs_out;

uniform sampler2D textureMask;

void main()
{
	vs_out.skip = true;
	vs_out.color = texture(textureMask, textureCoords).rgb;//vec3(255, 0, 0);
	if (vs_out.color.g == 1)
	{
		vs_out.skip = false;
	}

	vs_out.position = mdlVertexPos;

	//gl_Position = position;
}