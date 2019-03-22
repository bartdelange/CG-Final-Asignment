#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "types.h"


class ModelRenderer;
typedef void(*transFunc)(ModelRenderer &model);

class ModelRenderer
{
private:
	ObjectUniforms uniforms;
	LightSource light_source;
	Material material;
	int has_texture = 0;

	// The model itself (vertices, normals, uvs, ...)
	Mesh mesh;

	// Transformation
	bool should_transform;
	transFunc tranformFunc;

	// Shader related
	GLuint shader_id;
	GLuint vao;
	GLuint texture_id;


	void InitShaders();
	void InitBuffers();
	void TranformObject();
	void DrawModel();
	void FillUniforms();
public:
	ModelRenderer(const char * name, glm::mat4 model, glm::mat4 projection, glm::mat4 mv, LightSource light_source);
	std::string model_name;

	// Position and transformations
	glm::mat4 projection;
	glm::mat4 mv;
	glm::mat4 model;


	void Initialize();
	void ParseObject(const char * objectPath);
	void SetTexture(const char * texturePath);
	void SetMaterial(Material mat);
	void EnableTransformation(transFunc func);
	void DisableTransformation();
	void UpdateView(glm::mat4 proj, glm::mat4 mv);
	void Render();
};