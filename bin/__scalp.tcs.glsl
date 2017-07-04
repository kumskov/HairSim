#version 400 core 

layout (vertices = 3) out;

in VS_OUT
{
	vec3 color;
	bool skip;
} tc_in[];

//patch out vec3 patchColor;
//patch out bool patchSkip;

out TC_OUT
{
	vec3 color;
	bool skip;
} tc_out[];

void main(void) 
{ 
	if (gl_InvocationID == 0) 
	{ 
		gl_TessLevelInner[0] = 5.0;
		gl_TessLevelOuter[0] = 5.0;
		gl_TessLevelOuter[1] = 5.0;
		gl_TessLevelOuter[2] = 5.0;
	}
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	
	tc_out[gl_InvocationID].color = tc_in[gl_InvocationID].color;
	tc_out[gl_InvocationID].skip = tc_in[gl_InvocationID].skip;

	//patchColor = tc_in[gl_InvocationID].color;
	//patchSkip = tc_in[gl_InvocationID].skip;
} 