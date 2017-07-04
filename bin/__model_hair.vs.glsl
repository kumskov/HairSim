#version 400 core

layout(location = 0) in vec3 mdl_vertexPos;
//layout(location = 1) in vec3 hairColor;

uniform mat4 mvpMatrix;

out VS_OUT
{
	//float skip;
	vec3 color;
} vs_out; 

void main()
{
	/*
	if (texture(modelTexture, vertexTexCoords).rgb == vec3(0, 255, 0))
	{
		vs_out.skip = 0.0;
	}
	else
	{
		vs_out.skip = 1.0;
	}
	*/

	vs_out.color = vec3(0, 255, 0);

	gl_Position = mvpMatrix * vec4(mdl_vertexPos, 1);
}