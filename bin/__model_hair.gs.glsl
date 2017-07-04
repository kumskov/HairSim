#version 400 core

layout (triangles) in; 
layout (triangle_strip, max_vertices = 24) out; 

in VS_OUT
{
	//float skip;
	vec3 color;
} gs_in[]; 

out GS_OUT 
{
	vec3 color;
} gs_out;

uniform float hairLength = 10;
uniform float hairUnitLength = 3.5;

void main(void)
{	
//return;

	vec3 s1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 s2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;

	//get face centroid and face normal
	vec3 faceNormal = -normalize(cross(s1, s2));
	vec4 faceCenter = vec4( (gl_in[0].gl_Position.x + gl_in[1].gl_Position.x + gl_in[2].gl_Position.x)/3,
							(gl_in[0].gl_Position.y + gl_in[1].gl_Position.y + gl_in[2].gl_Position.y)/3,
							(gl_in[0].gl_Position.z + gl_in[1].gl_Position.z + gl_in[2].gl_Position.z)/3,
							0.0);

	//DEBUG: emitting scalp
	///*
	gl_Position = gl_in[0].gl_Position;
	gs_out.color = gs_in[0].color; 
	EmitVertex();
	gl_Position = gl_in[1].gl_Position;
	gs_out.color = gs_in[1].color; 
	EmitVertex();
	gl_Position = gl_in[2].gl_Position;
	gs_out.color = gs_in[2].color; 
	EmitVertex();
	EndPrimitive();
	//*/

	/*
	for (i = 0; i < 2; i++) 
	{
		gl_Position = faceCenter + vec4(faceNormal * i * hairUnitLength, 0.0);
		gs_out.color = vec3(255, 0, 255); 
		EmitVertex();
	}
	EndPrimitive(); 
	*/


	///*
	vec4[10] hairString;
	for (int i = 0; i < 5; ++i)
	{
		if (i == 0)
		{
			hairString[0] = gl_in[1].gl_Position;
			hairString[1] = gl_in[0].gl_Position;
		}
		else
		{
			hairString[i*2] = hairString[(i-1)*2] + vec4(faceNormal * hairUnitLength, 0.0);
			hairString[i*2+1] = hairString[(i-1)*2+1] + vec4(faceNormal * hairUnitLength, 0.0);
		}
	}

	
	int counter = 0;
	while(counter < 8)
	{
		for (int i = 0; i < 3; ++i)
		{
			gl_Position = hairString[counter+i];
			//gs_out.color = gs_in[0].color; 
			gs_out.color = vec3(255, 0, 0);
			EmitVertex();
		}
		EndPrimitive();
		counter++;
	}
	//*/
	
	/*
	for (int i = 0; i < hairLength/hairUnitLength; i++) 
	{
		gl_Position = faceCenter + vec4(faceNormal * hairUnitLength * i, 0.0);
		gs_out.color = gs_in[0].color; 
		EmitVertex();
	}
	*/
}