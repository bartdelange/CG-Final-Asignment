#include "Player.h"


/// <summary>
/// Resets the world to normal viewing mode
/// </summary>
void Player::ResetEagleEye()
{
	this->position = this->old_position;
	this->z_angle = this->old_z_angle;
	this->y_angle = this->old_y_angle;
}


/// <summary>
/// Recalculates all needed vectors:
/// - player_center
/// - player_side
/// - player_height
/// </summary>
void Player::CalculateVectors()
{
	// Calculate the new head position
	glm::vec3 newPlayerCenter;
	newPlayerCenter.x = cos(glm::radians(this->z_angle)) * cos(glm::radians(this->y_angle));
	newPlayerCenter.y = sin(glm::radians(this->y_angle));
	newPlayerCenter.z = sin(glm::radians(this->z_angle)) * cos(glm::radians(this->y_angle));
	this->player_center = glm::normalize(newPlayerCenter);


	// Normalize side and height vectors to keep an even movement
	this->player_side = glm::normalize(glm::cross(this->player_center, this->world_height));
	this->player_height = glm::normalize(glm::cross(this->player_side, this->player_center));
}


/// <summary>
/// Initializes everything
/// </summary>
/// <param name="position"></param>
/// <param name="playerHeight"></param>
/// <param name="zAngle"></param>
/// <param name="yAngle"></param>
Player::Player(glm::vec3 position, glm::vec3 playerHeight, float zAngle, float yAngle) :
	player_center(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed(SPEED), mouse_speed(MOUSESPEED)
{
	this->position = position;
	this->world_height = playerHeight;
	this->z_angle = zAngle;
	this->y_angle = yAngle;
	CalculateVectors();
}


void Player::SetMaxBounds(float minX, float maxX, float minZ, float maxZ)
{
	this->maxX = minX;
	this->maxX = maxX;
	this->maxZ = minZ;
	this->maxZ = maxZ;
}

/// <summary>
/// Returns the point the player is looking at
/// </summary>
/// <returns></returns>
glm::mat4 Player::LookingAt()
{
	return glm::lookAt(position, position + player_center, player_height);
}


/// <summary>
/// Moves the player
/// </summary>
/// <param name="direction"></param>
/// <param name="deltaTime"></param>
void Player::Move(MovementDirections direction, float deltaTime)
{
	// Reset eagle eye if we move again
	if (eagle_eye_enabled)
	{
		ResetEagleEye();
		CalculateVectors();
		this->eagle_eye_enabled = false;
	}

	const float velocity = movement_speed * deltaTime;

	// Where do we need to move to
	switch (direction)
	{
	case FORWARD:
		this->position += this->player_center * velocity;
		break;
	case BACKWARD:
		this->position -= this->player_center * velocity;
		break;
	case LEFT:
		this->position -= this->player_side * velocity;
		break;
	case RIGHT:
		this->position += this->player_side * velocity;
		break;
	}


	// Restrict player in fields
	if (this->position.x > maxX)
		this->position.x = maxX;
	if (this->position.z > maxZ)
		this->position.z = maxZ;
	if (this->position.z < minX)
		this->position.z = minX;
	if (this->position.z < minZ)
		this->position.z = minZ;

	// Locks the player at ground level
	this->position.y = 0.0f;
}


/// <summary>
/// Tilts and rotates the player's head
/// </summary>
/// <param name="xoffset"></param>
/// <param name="yoffset"></param>
/// <param name="constrainPitch"></param>
void Player::Look(float xoffset, float yoffset)
{
	xoffset *= this->mouse_speed;
	yoffset *= this->mouse_speed;

	this->z_angle += xoffset;
	this->y_angle += yoffset;

	// Keep y angle in screen bounds, rotating your head 90+ degrees backwards hurts a lot... trust me...
	if (this->y_angle > 89.0f)
		this->y_angle = 89.0f;
	if (this->y_angle < -89.0f)
		this->y_angle = -89.0f;

	CalculateVectors();
}


/// <summary>
/// Toggles the overview mode
/// </summary>
void Player::ToggleEagleEye()
{
	if (!this->eagle_eye_enabled)
	{
		this->old_position = this->position;
		this->old_z_angle = this->z_angle;
		this->old_y_angle = this->y_angle;

		this->z_angle = 60;
		this->y_angle = -25;
		this->position = glm::vec3(-20, 15, 10);
	}
	else
	{
		ResetEagleEye();
	}

	CalculateVectors();
	this->eagle_eye_enabled = !this->eagle_eye_enabled;
}