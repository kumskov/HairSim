#version 400 core                

in GS_OUT 
{
	vec3 color;
} fs_in;

out vec3 output_color;           

void main(void)
{
	//output_color = vec3(0, 255, 0);
	output_color = fs_in.color;
}