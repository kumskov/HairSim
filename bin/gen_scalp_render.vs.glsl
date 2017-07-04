#version 400 core

layout(location = 0) in vec3 mdlVertexPos;
layout(location = 1) in vec2 textureCoords;

/*
out VS_OUT
{
	vec3 color;
	bool skip;
	vec3 position;
} vs_out;
*/

out vec2 vs_texCoords;

uniform sampler2D textureMask;
uniform mat4 mvpMatrix;

void main()
{
	vs_texCoords = textureCoords;

	vec4 position = vec4(mdlVertexPos, 1);

	gl_Position = mvpMatrix * position;

	/*
	vs_out.skip = false;
	vs_out.color = texture(textureMask, textureCoords).rgb;//vec3(255, 0, 0);
	if (vs_out.color.g == 1)
	{
		vs_out.skip = false;
	}

	vs_out.position = mdlVertexPos;

	gl_Position = mvpMatrix * vec4(mdlVertexPos, 1);
	*/
}