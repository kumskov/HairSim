#version 400 core

layout(location = 0) in vec4 hairVertexPos;
//layout(location = 1) in vec4 hairPlane;

uniform mat4 mvpMatrix;
uniform vec4 hairPlane;


uniform vec3 hairColor;

out vec3 vs_color;

int random(int seed, int iterations)
{ 
	int value = seed; 
	int n;

	for (n = 0; n < iterations; n++) 
	{
		value = ((value >> 7) ^ (value << 9)) * 15485863; 
	} 

	return value; 
} 

void main()
{	
	vs_color = hairColor;
	float xoffset = ((gl_InstanceID - 60) / 10);
	float zoffset = (random(gl_InstanceID, 2) % 7);
	
	vec4 fixedPosition = hairVertexPos;

	vec4 offset = vec4(xoffset, 0.0, zoffset, 0.0);

	fixedPosition += offset;
	
	gl_Position = mvpMatrix * fixedPosition;

	//gl_Position = mvpMatrix * hairVertexPos;
	vs_color = hairColor;
	
}