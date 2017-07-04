#version 400 core 

layout (triangles, equal_spacing, cw) in; 

in TC_OUT
{
	vec3 color;
	bool skip;
} te_in[];

//patch in vec3 patchColor;
//patch in bool patchSkip;

out TE_OUT
{
	vec3 color;
	bool skip;
} te_out;

uniform mat4 mvpMatrix;

void main(void) 
{ 
	vec3 p1 = gl_TessCoord.x * gl_in[0].gl_Position.xyz;
	vec3 p2 = gl_TessCoord.y * gl_in[1].gl_Position.xyz;
	vec3 p3 = gl_TessCoord.z * gl_in[2].gl_Position.xyz;
	vec3 position = normalize(p1 + p2 + p3);

	te_out.color = te_in[0].color;
	te_out.skip = te_in[0].skip;

	gl_Position = mvpMatrix * vec4(position, 1.0);
} 