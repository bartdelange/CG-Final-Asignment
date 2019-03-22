#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glsl.h"
#include "objloader.hpp"
#include "texture.hpp"
#include "types.h"
#include "modelRenderer.h"
#include "player.h"

using namespace std;


const int WIDTH = 800;
const int HEIGHT = 600;
const int DELTA = 10;


float deltaTime = 0.0f;
float lastFrame = 0.0f;


vector<ModelRenderer> models;
LightSource lightSource;
GLuint program_id;

glm::mat4 iden, view, projection;
glm::mat4 mv;


Player player(glm::vec3(-5, 0, 8));


/// <summary>
/// Non repeatable key handler
/// </summary>
/// <param name="key"></param>
/// <param name="a"></param>
/// <param name="b"></param>
void keyboardHandler(unsigned char key, int a, int b)
{
    if (key == 27) // ESC
        glutExit();
	if (key == 99) // C.
		player.ToggleEagleEye();
}


/// <summary>
/// Repeatable key handler
/// </summary>
void OnKeyDown()
{
	if (GetAsyncKeyState(0x57)) // W.
		player.Move(FORWARD, deltaTime);
	if (GetAsyncKeyState(0x53)) // S.
		player.Move(BACKWARD, deltaTime);
	if (GetAsyncKeyState(0x41)) // A.
		player.Move(LEFT, deltaTime);
	if (GetAsyncKeyState(0x44)) // D.
		player.Move(RIGHT, deltaTime);
	if (GetAsyncKeyState(0x44)) // D.
		player.Move(RIGHT, deltaTime);
}

/// <summary>
/// Moves the players eyes according to the mouse position
/// This fixes the mouse on the center of the screen
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
void OnMouseMove(int x, int y)
{
	float centerX = WIDTH / 2;
	float centerY = HEIGHT / 2;
	player.Look(x - centerX, centerY - y);

	// Fix cursor in the center.
	if (x != centerY || y != centerX)
	{
		// Remove the mouse
		glutSetCursor(GLUT_CURSOR_NONE);
		glutWarpPointer(centerX, centerY);
	}
}


/// <summary>
/// This renders all models
/// </summary>
void Render()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	OnKeyDown();

	// Timing
	const float currentFrame = glutGet(GLUT_ELAPSED_TIME) / 100.0f;
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	view = player.LookingAt();
	projection = glm::perspective(glm::radians(45.0f), float(WIDTH) / HEIGHT, 0.1f, 100.0f);

	for (auto & model : models) {
		model.UpdateView(projection, view * model.model);
		model.Render();
	}

	glutSwapBuffers();
}


/// <summary>
/// The main game loop
/// </summary>
/// <param name="n"></param>
void Render(int n)
{
    Render();
    glutTimerFunc(DELTA, Render, 0);
}


/// <summary>
/// Initializes glut and glew
/// This also sets a few game settings like anti aliasing
/// </summary>
/// <param name="argc"></param>
/// <param name="argv"></param>
void InitGlutGlew(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutSetOption(GLUT_MULTISAMPLE, 16);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Bart de Lange: Straat");

	glutMotionFunc(OnMouseMove);
	glutPassiveMotionFunc(OnMouseMove);

	glutDisplayFunc(Render);
	glutKeyboardFunc(keyboardHandler);
	glutTimerFunc(DELTA, Render, 0);

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);

	glewInit();
}


/// <summary>
/// Transform function 1 and 2
/// </summary>
/// <param name="model"></param>
void transformM1(ModelRenderer &model) {
	model.model = glm::rotate(model.model, .01f, glm::vec3(1.0f, 1.0f, 1.0f));
}
void transformM2(ModelRenderer &model) {
	model.model = glm::rotate(model.model, .01f, glm::vec3(0.0f, 1.0f, 0.0f));
}


/// <summary>
/// Initializes the models that need to be rendered
/// </summary>
void InitModels()
{
	glm::mat4 model;

	// Init model 1 
	model = glm::translate(iden, glm::vec3(10, 0, -2 * 31));
	ModelRenderer &m1 = ModelRenderer("Cube", model, projection, view * model, lightSource);
	m1.ParseObject("Objects/box.obj");
	m1.SetMaterial(Material{
		glm::vec3(0.2, 0.2, 0.0),
		glm::vec3(0.5, 0.5, 0.0),
		glm::vec3(2, 2, 2),
		128
		});
	m1.SetTexture("Textures/Yellobrk.bmp");
	m1.EnableTransformation(&transformM1);
	m1.Initialize();
	models.push_back(m1);
	
	// Init model 2
	model = glm::translate(iden, glm::vec3(-10, 0, -2 * 31));
	ModelRenderer &m2 = ModelRenderer("Teapot", model, projection, view * model, lightSource);
	m2.ParseObject("Objects/teapot.obj");
	m2.SetMaterial(Material{
		glm::vec3(0.5, 0.0, 0.0),
		glm::vec3(1, 0.0, 0.0),
		glm::vec3(5, 5, 5),
		128
	});
	m2.EnableTransformation(&transformM2);
	m2.Initialize();
	models.push_back(m2);


	for (int i = 0; i < 31; i++)
	{
		m2.model = glm::translate(m2.model, glm::vec3(0, 0, 2));
		models.push_back(m2);
	}

}


int main(int argc, char ** argv)
{
    InitGlutGlew(argc, argv);
	lightSource.position = glm::vec3(4.0, 4.0, 4.0);
    InitModels();

    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_SHOW);

    glutMainLoop();

    return 0;
}
