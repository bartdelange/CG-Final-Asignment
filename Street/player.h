#pragma once
#include <glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum MovementDirections
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const float Z_ANGLE = -90;
const float Y_ANGLE = 0;
const float SPEED = .5f;
const float MOUSESPEED = 0.1f;
const float ZOOM = 45.0f;

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Player
{
private:
	glm::vec3 old_position;
	float old_z_angle;
	float old_y_angle;
	bool eagle_eye_enabled = false;

	float maxX = 100;
	float maxZ = 100;
	float minX = 0;
	float minZ = 0;
	
	void ResetEagleEye();
	void CalculateVectors();

public:
	Player(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 playerHeight = glm::vec3(0.0f, 1.0f, 0.0f), float zAngle = Z_ANGLE, float yAngle = Y_ANGLE);

	glm::vec3 position;
	glm::vec3 player_center;
	glm::vec3 player_height;
	glm::vec3 player_side;
	glm::vec3 world_height;

	float z_angle;
	float y_angle;

	float movement_speed;
	float mouse_speed;

	void SetMaxBounds(float minX, float maxX, float minZ, float maxZ);
	glm::mat4 LookingAt();
	void Move(MovementDirections direction, float deltaTime);
	void Look(float xoffset, float yoffset);
	void ToggleEagleEye();
};