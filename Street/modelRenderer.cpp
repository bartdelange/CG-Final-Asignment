#include <vector>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glsl.h"
#include "objloader.hpp"
#include "texture.hpp"
#include "modelRenderer.h"

const char * fragshader_name = "fragmentshader.fsh";
const char * vertexshader_name = "vertexshader.vsh";


/// <summary>
/// ctor
/// </summary>
/// <param name="name">Name of the modal</param>
/// <param name="model">Where we are looking from/at</param>
/// <param name="projection">Where we are looking from/at</param>
/// <param name="mv">Mat4 view * Model</param>
/// <param name="lightSource"></param>
ModelRenderer::ModelRenderer(const char * name, glm::mat4 model, glm::mat4 projection, glm::mat4 mv, LightSource lightSource)
{
	this->model_name = name;
	this->model = model;
	this->projection = projection;
	this->mv = mv;
	this->light_source = lightSource;

	InitShaders();
}


/// <summary>
/// Inititalizes the shader files used by the model
/// </summary>
void ModelRenderer::InitShaders()
{
	char * vertexshader = glsl::readFile(vertexshader_name);
	GLuint vsh_id = glsl::makeVertexShader(vertexshader);

	char * fragshader = glsl::readFile(fragshader_name);
	GLuint fsh_id = glsl::makeFragmentShader(fragshader);

	this->shader_id = glsl::makeShaderProgram(vsh_id, fsh_id);
}


/// <summary>
/// Transforms the model if transformations are enabled
/// </summary>
void ModelRenderer::TranformObject()
{
	if (should_transform)
		tranformFunc(*this);
}


/// <summary>
/// Draws the modal
/// </summary>
void ModelRenderer::DrawModel()
{
	// Rebind texture
	glBindTexture(GL_TEXTURE_2D, this->texture_id);

	// Send vao
	glBindVertexArray(this->vao);
	glDrawArrays(GL_TRIANGLES, 0, this->mesh.vertices.size());
	glBindVertexArray(0);
}


/// <summary>
/// Fills all needed uniforms
/// </summary>
void ModelRenderer::FillUniforms()
{
	glUseProgram(this->shader_id);
	glUniformMatrix4fv(this->uniforms.mv, 1, GL_FALSE, glm::value_ptr(this->mv));
	glUniformMatrix4fv(this->uniforms.proj, 1, GL_FALSE, glm::value_ptr(this->projection));
	glUniform3fv(this->uniforms.light_pos, 1, glm::value_ptr(this->light_source.position));
	glUniform3fv(this->uniforms.material_ambient, 1, glm::value_ptr(this->material.ambient_color));
	glUniform3fv(this->uniforms.material_diffuse, 1, glm::value_ptr(this->material.diffuse_color));
	glUniform3fv(this->uniforms.material_specular, 1, glm::value_ptr(this->material.specular));
	glUniform1f(this->uniforms.material_power, this->material.power);
	glUniform1i(this->uniforms.has_texture, this->has_texture);
}


/// <summary>
/// Initializes the buffers used by the model
/// </summary>
void ModelRenderer::InitBuffers()
{
	GLuint position_id;
	GLuint normal_id;
	GLuint uv_id;
	GLuint vbo_vertices;
	GLuint vbo_normals;
	GLuint vbo_uvs;

	// vbo for vertices
	glGenBuffers(1, &vbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, this->mesh.vertices.size() * sizeof(glm::vec3), &(this->mesh.vertices[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// vbo for normals
	glGenBuffers(1, &vbo_normals);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
	glBufferData(GL_ARRAY_BUFFER, this->mesh.normals.size() * sizeof(glm::vec3), &(this->mesh.normals[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// vbo for uvs
	glGenBuffers(1, &vbo_uvs);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs);
	glBufferData(GL_ARRAY_BUFFER, this->mesh.uvs.size() * sizeof(glm::vec2), &(this->mesh.uvs[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Get vertex attributes
	position_id = glGetAttribLocation(this->shader_id, "position");
	normal_id = glGetAttribLocation(this->shader_id, "normal");
	uv_id = glGetAttribLocation(this->shader_id, "uv");

	// Allocate memory for vao
	glGenVertexArrays(1, &this->vao);

	// Init binding to the vao
	glBindVertexArray(this->vao);

	// Bind vertices to the vao
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glVertexAttribPointer(position_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(position_id);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
	glVertexAttribPointer(normal_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(normal_id);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs);
	glVertexAttribPointer(uv_id, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(uv_id);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Stop binding to the vao
	glBindVertexArray(0);

	glUseProgram(this->shader_id);

	// Save uniform variables
	this->uniforms.mv = glGetUniformLocation(this->shader_id, "mv");
	this->uniforms.proj = glGetUniformLocation(this->shader_id, "projection");
	this->uniforms.light_pos = glGetUniformLocation(this->shader_id, "light_pos");
	this->uniforms.material_ambient = glGetUniformLocation(this->shader_id, "mat_ambient");
	this->uniforms.material_diffuse = glGetUniformLocation(this->shader_id, "mat_diffuse");
	this->uniforms.material_specular = glGetUniformLocation(this->shader_id, "mat_specular");
	this->uniforms.material_power = glGetUniformLocation(this->shader_id, "mat_power");
	this->uniforms.has_texture = glGetUniformLocation(this->shader_id, "has_texture");
}


/// <summary>
/// Initializes the model
/// </summary>
void ModelRenderer::Initialize()
{
	// This currently only initializes the buffers but can be use to call more stuff when all vars are set
	this->InitBuffers();
}


/// <summary>
/// Parses an obj file and fill the mesh
/// </summary>
/// <param name="object_path">The path to the obj file</param>
void ModelRenderer::ParseObject(const char * object_path)
{
	vector<glm::vec3> vertices;
	vector<glm::vec3> normals;
	vector<glm::vec2> uvs;

	bool res = loadOBJ(object_path, vertices, uvs, normals);

	this->mesh = Mesh{ vertices, normals, uvs };
}


/// <summary>
/// Loads the texture the model will use
/// </summary>
/// <param name="texture_path">The path to the textue</param>
void ModelRenderer::SetTexture(const char * texture_path)
{
	this->has_texture = 1;
	this->texture_id = loadBMP(texture_path);
}


/// <summary>
/// Sets the material used by the model
/// </summary>
/// <param name="mat">The material</param>
void ModelRenderer::SetMaterial(Material mat)
{
	this->material = mat;
}


/// <summary>
/// Enables the use of looped transformations
/// </summary>
/// <param name="func">The transformation that executes every loop</param>
void ModelRenderer::EnableTransformation(transFunc func)
{
	this->should_transform = true;
	this->tranformFunc = func;
}


/// <summary>
/// Disables transformation
/// </summary>
void ModelRenderer::DisableTransformation()
{
	this->should_transform = false;
}


/// <summary>
/// Updates the matrices needed to project the object to a players f.o.v.
/// </summary>
/// <param name="proj"></param>
/// <param name="mv"></param>
void ModelRenderer::UpdateView(glm::mat4 proj, glm::mat4 mv)
{
	this->projection = proj;
	this->mv = mv;
}


/// <summary>
/// Renders the model
/// </summary>
void ModelRenderer::Render()
{
	this->TranformObject();
	this->FillUniforms();
	this->DrawModel();
}

