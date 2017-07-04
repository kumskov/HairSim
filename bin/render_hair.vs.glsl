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
	
	
	float angle = (gl_InstanceID % 360) * 10;
	float rad = (1 + (gl_InstanceID / 360 ) * 4) * 2;
	
	vec4 fixedPosition = hairVertexPos;

	float xPlaneTransform = rad * cos(angle * 0.01745329) + (random(gl_InstanceID, 2) % 5)/7;
	float zPlaneTransform = rad * sin(angle * 0.01745329) + (random(gl_InstanceID + random(gl_InstanceID, 1) % 10, 3) % 7)/5;
	
	float yPlaneTransform = 0;
	/*
	yPlaneTransform = (-1)*((fixedPosition.x + xPlaneTransform) * hairPlane.x 
						+ (fixedPosition.z + zPlaneTransform) * hairPlane.z 
						+ hairPlane[3])/hairPlane.y;
	*/

	vec4 offset = vec4(xPlaneTransform, yPlaneTransform, zPlaneTransform, 0.0);

	fixedPosition += offset;
	/*
	fixedPosition.x += xPlaneTransform;
	fixedPosition.z += zPlaneTransform;
	fixedPosition.y += yPlaneTransform;
	*/
	//fixedPosition.y += hairPlane.y * hairPlane[3] * 10;
	
	
	
	gl_Position = mvpMatrix * fixedPosition;

	//gl_Position = mvpMatrix * hairVertexPos;
	vs_color = hairColor;
	
}