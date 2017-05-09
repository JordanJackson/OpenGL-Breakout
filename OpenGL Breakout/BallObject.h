#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Texture2D.h"
#include "SpriteRenderer.h"
#include "GameObject.h"

class BallObject : public GameObject
{
public:
	// Ball state
	GLfloat Radius;
	GLboolean Stuck;

public:
	BallObject();
	BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite);

	glm::vec2		Move(GLfloat dt, GLuint windowWidth);
	void			Reset(glm::vec2 position, glm::vec2 velocity);
};

