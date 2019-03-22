#pragma once
#include <vector>
#include <glm/glm.hpp>

struct Mesh
{
	vector<glm::vec3> vertices;
	vector<glm::vec3> normals;
	vector<glm::vec2> uvs;
};

struct LightSource
{
	glm::vec3 position;
};

struct Material
{
	glm::vec3 ambient_color;
	glm::vec3 diffuse_color;
	glm::vec3 specular;
	float power;
};

struct ObjectUniforms {
	GLuint proj;
	GLuint mv;
	GLuint light_pos;
	GLuint material_ambient;
	GLuint material_diffuse;
	GLuint material_specular;
	GLuint material_power;
	GLuint has_texture;
};
