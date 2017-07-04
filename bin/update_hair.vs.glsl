#version 400 core

layout (location = 0) in vec4 current;
layout (location = 1) in vec4 previous;
layout (location = 2) in vec3 velocity;

uniform samplerBuffer mdlVertices;

out VS_OUT
{
	vec4 position;
	vec3 velocity;
} vs_out; 

// A uniform to hold the timestep. The application can update this.
uniform float t = 0.07;

const vec3 gravity = vec3(0.0, -0.08, 0.0);

uniform float stringLength;
uniform float mass;
uniform vec3 force;

void main(void)
{
	float invMass = 1.0/mass;
	vec3 newVelocity = velocity + t * (force * invMass);
	vec4 curTmpPosition = current + vec4(velocity * t, 1.0);
	newVelocity = newVelocity*0.99;

	//pb = current
	//pa = previous
	vec4 direction = curTmpPosition - previous;
	normalize(direction);
	vec4 curTmpPosition2 = curTmpPosition;
	curTmpPosition = curTmpPosition + direction * stringLength;
	curTmpPosition = curTmpPosition2 - curTmpPosition; //  - curr_pos;

	vs_out.velocity = newVelocity;
	vs_out.position = curTmpPosition;
}
