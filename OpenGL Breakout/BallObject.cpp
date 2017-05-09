#include "BallObject.h"



BallObject::BallObject()
	: GameObject(), Radius(12.5f), Stuck(true)
{

}

BallObject::BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite)
	: GameObject(pos, glm::vec2(radius * 2, radius * 2), sprite, glm::vec3(1.0f), velocity), Radius(radius), Stuck(true)
{

}

glm::vec2 BallObject::Move(GLfloat dt, GLuint windowWidth)
{
	// If not stuck to player paddle
	if (!this->Stuck)
	{
		// Move the ball
		this->Position += this->Velocity * dt;

		// Check if outside window bounds
		if (this->Position.x <= 0.0f)
		{
			this->Velocity.x = -this->Velocity.x;
			this->Position.x = 0.0f;
		}
		else if (this->Position.x + this->Size.x >= windowWidth)
		{
			this->Velocity.x = -this->Velocity.x;
			this->Position.x = windowWidth - this->Size.x;
		}

		if (this->Position.y <= 0.0f)
		{
			this->Velocity.y = -this->Velocity.y;
			this->Position.y = 0.0f;
		}
	}

	return this->Position;
}

// Resets the ball to initial Stuck Position
void BallObject::Reset(glm::vec2 position, glm::vec2 velocity)
{
	this->Position = position;
	this->Velocity = velocity;
	this->Stuck = true;
}