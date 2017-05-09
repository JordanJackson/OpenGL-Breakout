#include "CollisionHandler.h"

GLboolean CollisionHandler::CheckCollision(GameObject& one, GameObject& two)
{
	// Collision only if on both axes
	return (
		(one.Position.x + one.Size.x >= two.Position.x && two.Position.x + two.Size.x >= one.Position.x)		// X-axis
		&& (one.Position.y + one.Size.y >= two.Position.y && two.Position.y + two.Size.y >= one.Position.y)		// Y-axis
		);
}

Collision CollisionHandler::CheckCollision(BallObject& one, GameObject& two)
{
	// Get center point circle first
	glm::vec2 center(one.Position + one.Radius);
	// Calculate AABB info (center, half-extents)
	glm::vec2 aabbHalfExtents(two.Size.x * 0.5f, two.Size.y * 0.5f);
	glm::vec2 aabbCenter(two.Position.x + aabbHalfExtents.x, two.Position.y + aabbHalfExtents.y);

	// Get difference vector between both centers
	glm::vec2 difference = center - aabbCenter;
	glm::vec2 clamped = glm::clamp(difference, -aabbHalfExtents, aabbHalfExtents);
	// Now that we know the clamped values, add this to AABB center and we get point on box closest to circle
	glm::vec2 closest = aabbCenter + clamped;
	// Now retrieve vector between circle and closest point AABB and check if length < radius
	difference = closest - center;

	if (glm::length(difference) < one.Radius)	// not <= due to == being at collision resolution
	{
		return std::make_tuple(true, VectorDirection(difference), difference);
	}
	else
	{
		return std::make_tuple(false, UP, glm::vec2(0, 0));
	}
}

Direction CollisionHandler::VectorDirection(glm::vec2 target)
{
	glm::vec2 compass[] =
	{
		glm::vec2(0.0f, 1.0f),		// up
		glm::vec2(1.0f, 0.0f),		// right
		glm::vec2(0.0f, -1.0f),		// down
		glm::vec2(-1.0f, 0.0f)		// left
	};

	GLfloat max = 0.0f;
	GLuint bestMatch = -1;
	for (GLuint i = 0; i < 4; i++)
	{
		GLfloat dotProduct = glm::dot(glm::normalize(target), compass[i]);
		if (dotProduct > max)
		{
			max = dotProduct;
			bestMatch = i;
		}
	}

	return (Direction)bestMatch;
}