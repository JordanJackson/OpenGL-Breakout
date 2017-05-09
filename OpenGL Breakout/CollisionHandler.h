#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <tuple>

#include "BallObject.h"
#include "GameObject.h"

enum Direction
{
	UP,
	RIGHT,
	DOWN,
	LEFT,
};

// Defines a Collision typedef containing Collision info
typedef std::tuple<bool, Direction, glm::vec2> Collision;

class CollisionHandler
{
public:
	static GLboolean CheckCollision(GameObject& one, GameObject& two);
	static Collision CheckCollision(BallObject& one, GameObject& two);
private:
	CollisionHandler() {};
	static Direction VectorDirection(glm::vec2 target);
};

