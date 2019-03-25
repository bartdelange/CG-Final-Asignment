#include <iostream>
#include <vector>
#include <algorithm>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
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


Player player;


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
	glutCreateWindow("Bart de Lange: RainbowLane");

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
/// Creates two "simple" houses
/// </summary>
void CreateHouses()
{
	glm::mat4 model;

	// Init house 1
	model = glm::translate(iden, glm::vec3(0, -0.9, 0));
	ModelRenderer &house1 = ModelRenderer("House1", model, projection, view * model, lightSource);
	house1.ParseObject("Objects/house1.obj");
	house1.SetMaterial(Material{
		glm::vec3(0.2, 0.2, 0.2),
		glm::vec3(0.9, 0.9, 0.9),
		glm::vec3(1.0, 1.0, 1.0),
		128
		});
	// The texture maping does not work in gl while it does in blender, opengl, windows 3dviewer, ...
	house1.SetTexture("Textures/house1.bmp");
	house1.Initialize();
	models.push_back(house1);

	// Init house 2
	model = glm::translate(iden, glm::vec3(0.5, -1.3, 5));
	model = glm::scale(model, glm::vec3(0.5));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	ModelRenderer &house2 = ModelRenderer("House2", model, projection, view * model, lightSource);
	house2.ParseObject("Objects/house2.obj");
	house2.SetMaterial(Material{
		glm::vec3(0.2, 0.2, 0.2),
		glm::vec3(0.9, 0.9, 0.9),
		glm::vec3(1.0, 1.0, 1.0),
		128
		});
	house2.SetTexture("Textures/house2.bmp"); // Same texture issue
	house2.Initialize();
	models.push_back(house2);

	// Repeat them a couple of times
	for (int i = 0; i < 15; i++)
	{
		float spacing = 12.5f;

		house1.model = glm::translate(house1.model, glm::vec3(0, 0, spacing));
		house2.model = glm::translate(house2.model, glm::vec3(spacing * 2, 0, 0));
		
		models.push_back(house1);
		models.push_back(house2);
	}
}


/// <summary>
/// Creates a road with lamp posts next to it (on the oposite of the houses and point towards them)
/// </summary>
void CreateRoad()
{
	glm::mat4 model;

	// Create plane for the houses
	model = glm::translate(iden, glm::vec3(1.5, -1.8, 0));
	model = glm::scale(model, glm::vec3(4));
	ModelRenderer &brick = ModelRenderer("Brick", model, projection, view * model, lightSource);
	brick.ParseObject("Objects/street.obj");
	brick.SetMaterial(Material{
		glm::vec3(0.5, 0.0, 0.0),
		glm::vec3(1.0, 0.0, 0.0),
		glm::vec3(1.0, 1.0, 1.0),
		128
	});
	brick.SetTexture("Textures/yellowbrick.bmp");
	brick.Initialize();
	models.push_back(brick);
	for (int i = 0; i < 24; i++)
	{
		float spacing = 2.0f;

		brick.model = glm::translate(brick.model, glm::vec3(0, 0, spacing));

		models.push_back(brick);
	}


	// Create road
	model = glm::translate(iden, glm::vec3(-6.5, -1.8, 0));
	model = glm::scale(model, glm::vec3(4));
	ModelRenderer &street = ModelRenderer("Street", model, projection, view * model, lightSource);
	street.ParseObject("Objects/street.obj");
	street.SetMaterial(Material{
		glm::vec3(0.5, 0.0, 0.0),
		glm::vec3(1.0, 0.0, 0.0),
		glm::vec3(1.0, 1.0, 1.0),
		128
	});
	street.SetTexture("Textures/street.bmp");
	street.Initialize();
	models.push_back(street);
	for (int i = 0; i < 24; i++)
	{
		float spacing = 2.0f;

		street.model = glm::translate(street.model, glm::vec3(0, 0, spacing));

		models.push_back(street);
	}


	// Create plane for next to the street
	model = glm::translate(iden, glm::vec3(-14.5, -1.8, 0));
	model = glm::scale(model, glm::vec3(4));
	ModelRenderer &brick2 = ModelRenderer("Brick", model, projection, view * model, lightSource);
	brick2.ParseObject("Objects/street.obj");
	brick2.SetMaterial(Material{
		glm::vec3(0.5, 0.0, 0.0),
		glm::vec3(1.0, 0.0, 0.0),
		glm::vec3(1.0, 1.0, 1.0),
		128
		});
	brick2.SetTexture("Textures/yellowbrick.bmp");
	brick2.Initialize();
	models.push_back(brick2);
	for (int i = 0; i < 24; i++)
	{
		float spacing = 2.0f;

		brick2.model = glm::translate(brick2.model, glm::vec3(0, 0, spacing));

		models.push_back(brick2);
	}


	// Create streetlamps
	model = glm::translate(iden, glm::vec3(-10, -1.45, 0));
	model = glm::scale(model, glm::vec3(0.6));
	ModelRenderer &lamppost = ModelRenderer("Lamppost", model, projection, view * model, lightSource);
	lamppost.ParseObject("Objects/lamppost.obj");
	lamppost.SetMaterial(Material{
		glm::vec3(0.5, 0.0, 0.0),
		glm::vec3(1.0, 0.0, 0.0),
		glm::vec3(1.0, 1.0, 1.0),
		128
	});
	lamppost.Initialize();
	models.push_back(lamppost);
	for (int i = 0; i < 6; i++)
	{
		float spacing = 12.5f;

		float r = ((float)(rand() % 255)) / 255; // red component of color
		float g = ((float)(rand() % 255)) / 255; // green component of color
		float b = ((float)(rand() % 255)) / 255; // blue component of color

		lamppost.model = glm::translate(lamppost.model, glm::vec3(0, 0, spacing * 4));
		lamppost.SetMaterial(Material{
			glm::vec3(r, g, b),
			glm::vec3(r + 0.2, g + 0.2, b + 0.2),
			glm::vec3(1.0, 1.0, 1.0),
			128
		});

		models.push_back(lamppost);
	}
}


/// <summary>
/// Creates the initial model matrix needed for the plane
/// </summary>
/// <returns></returns>
glm::mat4 CreatePaperMatrix()
{
	glm::mat4 model = glm::mat4();
	model = glm::scale(model, glm::vec3(100));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(0.0, 0.08, 0.5));
	return model;
}


/// <summary>
/// Flying animation for the paper plane
/// </summary>
/// <param name="model"></param>
bool is_positive_rotating = false;
bool passed_treshhold = true;
float rotation_speed = 0.5f;
void FlyAnim(ModelRenderer &model)
{
	// Decompose all transformation proterties into readable vars
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(model.model, scale, rotation, translation, skew, perspective);

	float rot = abs(rotation.y);

	if (rot > 0.5f) {
		if (rot > 0.9f)
			passed_treshhold = true;
		else
			passed_treshhold = false;
	}
	else
	{
		if (rot < 0.05f)
			passed_treshhold = true;
		else
			passed_treshhold = false;
	}


	// Wobble effect
	if (rot > 0.5f) 
	{
		if (rot < 0.99f)
			if (passed_treshhold)
				is_positive_rotating = !is_positive_rotating;
	}
	else
	{
		if (rot > 0.1f)
			if (passed_treshhold)
				is_positive_rotating = !is_positive_rotating;
	}

	if (is_positive_rotating)
		rotation_speed = abs(rotation_speed);
	else
		rotation_speed = -abs(rotation_speed);

	model.model = glm::rotate(model.model, glm::radians(rotation_speed), glm::vec3(0.0f, 1.0f, 0.0f));

	// Keep it moving
	if (translation.z > 250) {
		model.model = CreatePaperMatrix();
	}

	model.model = glm::translate(model.model, glm::vec3(0.0f, 0.0f, -0.005f));
}


/// <summary>
/// Creates a paper plane that flies around
/// </summary>
void CreatePaperPlane()
{
	// Create plane 
	glm::mat4 model = CreatePaperMatrix();
	ModelRenderer &plane = ModelRenderer("Brick", model, projection, view * model, lightSource);
	plane.ParseObject("Objects/paper_airplane.obj");
	plane.SetMaterial(Material{
		glm::vec3(0.5, 0.0, 0.0),
		glm::vec3(1.0, 0.0, 0.0),
		glm::vec3(1.0, 1.0, 1.0),
		128
	});
	plane.SetTexture("Textures/paper.bmp");
	plane.EnableTransformation(&FlyAnim);
	plane.Initialize();
	models.push_back(plane);
}



/// <summary>
/// Initializes the models that need to be rendered
/// </summary>
void InitModels()
{
	CreateHouses();
	CreateRoad();
	CreatePaperPlane();
}


int main(int argc, char ** argv)
{
    InitGlutGlew(argc, argv);
	lightSource.position = glm::vec3(-8.0, 2.0, 8.0);
	player = Player(glm::vec3(-5, 0, 100));
	player.SetMaxBounds(25, 175, 25, 175);
    InitModels();

    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_SHOW);

    glutMainLoop();

    return 0;
}
