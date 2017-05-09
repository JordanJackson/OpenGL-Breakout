#pragma once

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>

#include <vector>

#include "GameLevel.h"
#include "BallObject.h"
#include "CollisionHandler.h"

// Represents the current state of the game
enum GameState
{
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN,
};

// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability
class Game
{
public:
	// Game State
	GameState	State;
	GLboolean	Keys[1024];
	GLuint		Width, Height;
	std::vector<GameLevel>  Levels;
	GLuint					CurrentLevel;
	// Constructor/Destructor
	Game(GLuint width, GLuint height);
	virtual ~Game();
	// Initialize game state (load shaders/textures/levels)
	void Init();
	// GameLoop
	void ProcessInput(GLfloat dt);
	void Update(GLfloat dt);
	void Render();
private:
	void DoCollisions(GLfloat dt);
	void ResetLevel();
	void ResetPlayer();
};
