#version 400 core

layout (triangles) in; 
layout (triangle_strip, max_vertices = 3) out; 

in VS_OUT
{
	vec3 color;
	bool skip;
	vec3 position;
} gs_in[];

out GS_OUT 
{
	vec3 normal;
} gs_out; 

void main(void)
{	
	if ((gs_in[0].skip == false) &&
		(gs_in[1].skip == false) &&
		(gs_in[2].skip == false))
	{
		//vec3 s1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
		//vec3 s2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
		vec3 s1 = gs_in[1].position.xyz - gs_in[0].position.xyz;
		vec3 s2 = gs_in[2].position.xyz - gs_in[0].position.xyz;
		vec3 faceNormal = normalize(cross(s1, s2));

		for (int i = 0; i < 3; ++i)
		{
			gl_Position = vec4(gs_in[i].position, 1);
			//gs_out_color = gs_in[i].color;
			gs_out.normal = faceNormal;
			EmitVertex();
		}
		
	}

	/*
	for(int i = 0; i < gs_in.length(); ++i)
	{
		if (!gs_in[i].skip)
		{
			vec3 s1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
			vec3 s2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
			vec3 faceNormal = -normalize(cross(s1, s2));

			gl_Position = gl_in[i].gl_Position;
			//gs_out_color = gs_in[i].color;
			gs_out.normal = faceNormal;
			EmitVertex();
		}
	}
	EndPrimitive();
	*/
}