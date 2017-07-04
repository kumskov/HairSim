#ifndef __DATA_HPP__
#define __DATA_HPP__

#include <vector>
#include <vmath.h>

enum Transformation
{
	MOVE_UP,
	MOVE_DOWN,
	MOVE_LEFT,
	MOVE_RIGHT,
	MOVE_FORWARD,
	MOVE_BACKWARD,

	ROTATE_UP,
	ROTATE_DOWN,
	ROTATE_LEFT,
	ROTATE_RIGHT,
	ROTATE_Z_LEFT,
	ROTATE_Z_RIGHT
};

struct Material
{
	std::string name;
	std::string filename;
	GLuint 		texture;
};

struct MaterialFaces
{
	Material material;
	std::vector<vmath::vec3> vertices;
	std::vector<vmath::vec2> texCoords;
	std::vector<vmath::vec3> normals;
};


struct BoundingBox
{
	float xmin;
	float xmax;
	float ymin;
	float ymax;
	float zmin;
	float zmax;
};

struct BoundingSphere
{
	vmath::vec3 center;
	float rad;
};

struct CollisionBound
{
	BoundingSphere part1;
	BoundingSphere part2;
};

struct Model
{
	std::vector<MaterialFaces> materials;
};

struct ModelCollision
{
	std::vector<CollisionBound> bounds;
};

struct ModelFace
{
	//Ax + By + Cz + D = 0;
	float A;
	float B;
	float C;
	float D;
};


struct ScalpTFObject
{
	vmath::vec4 gl_Position;
	vmath::vec3 normal;
};

struct ScalpTFFaceObject
{
	//vmath::vec4 gl_Position;
	vmath::vec4 center;
	vmath::vec3 normal;
	ModelFace face_plane;
};

struct ScalpTFCollectorObject
{
	vmath::vec4 gl_Position;
	std::vector<vmath::vec3> normals;
};

struct HairObject
{
	std::vector<vmath::vec4> vertices;
	std::vector<vmath::vec4> forces;
	std::vector<vmath::vec4> velocities;
	vmath::vec3 normal;
	vmath::vec3 color;
	ModelFace plane;
};

struct HairBufferObject
{
	std::vector<vmath::vec4> vertices;
	std::vector<vmath::vec3> colors;
	int offset;
	std::vector<ModelFace> planes;
	std::vector<vmath::vec3> normals;
	std::vector<vmath::vec4> forces;
	std::vector<vmath::vec4> velocities;
};

struct HairTFObject
{
	float position;
	float velocity;
};

struct HairUpdateBufferObject
{
	std::vector<vmath::vec4> currentPosition;
	std::vector<vmath::vec4> prevPosition;
	std::vector<vmath::vec3> velocity;
	float mass;
	float length;
	vmath::vec3 force;
};

struct ModelConfigValues
{
	std::string model_value;
	std::string mask_value;
	std::string masked_material_value;
	std::string scalp_value;
	std::string use_value;
	std::string conf_value;
};

struct ConfigValues
{
	std::string mass_value;
	std::string hair_unit_length_value;
	std::string hair_length_value;
	std::string air_friction_value;
	std::string timestep_value;
	std::string collision_dist_value;
	std::string wind_v_step_value;
	std::string wind_h_step_value;
	std::string wind_speed_step_value;
	std::string cam_rot_speed_value;
	std::string cam_mv_speed_value;
	std::string arrow_model_fix_value;
};

#endif