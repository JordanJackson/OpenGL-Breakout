#include "Game.h"

#include "ParticleGenerator.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100, 20);
// Initial velocity of the player paddle
const GLfloat PLAYER_VELOCITY(500.0f);
// Initial velocity of the Ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// Radius of the ball object
const GLfloat BALL_RADIUS = 12.5f;

SpriteRenderer* Renderer;

GameObject* Player;
BallObject* Ball;

ParticleGenerator* ParticleGen;

Game::Game(GLuint width, GLuint height)
	: State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{

}

Game::~Game()
{
	delete Renderer;
	delete Player;
	delete Ball;
	delete ParticleGen;
}

void Game::Init()
{
	// Load shaders
	ResourceManager::LoadShader("shaders/sprite.vert", "shaders/sprite.frag", nullptr, "sprite");
	ResourceManager::LoadShader("shaders/particle.vert", "shaders/particle.frag", nullptr, "particle");
	// Configure shaders
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width), static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInteger("sprite", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
	ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
	ResourceManager::GetShader("particle").Use().SetMatrix4("projection", projection);
	// Load textures
	ResourceManager::LoadTexture("textures/background.jpg", GL_FALSE, "background");
	ResourceManager::LoadTexture("textures/awesomeface.png", GL_TRUE, "face");
	ResourceManager::LoadTexture("textures/block.png", GL_FALSE, "block");
	ResourceManager::LoadTexture("textures/block_solid.png", GL_FALSE, "block_solid");
	ResourceManager::LoadTexture("textures/paddle.png", GL_TRUE, "paddle");
	ResourceManager::LoadTexture("textures/particle.png", GL_TRUE, "particle");
	// Set render-specific controls
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

	ParticleGen = new ParticleGenerator(
		ResourceManager::GetShader("particle"),
		ResourceManager::GetTexture("particle"),
		500
	);

	// Load levels
	GameLevel one;
	one.Load("levels/one.lvl", this->Width, this->Height * 0.5);
	GameLevel two;
	two.Load("levels/two.lvl", this->Width, this->Height * 0.5);
	GameLevel three;
	three.Load("levels/three.lvl", this->Width, this->Height * 0.5);
	GameLevel four;
	four.Load("levels/four.lvl", this->Width, this->Height * 0.5);
	this->Levels.push_back(one);
	this->Levels.push_back(two);
	this->Levels.push_back(three);
	this->Levels.push_back(four);
	this->CurrentLevel = 0;

	// initialize player
	glm::vec2 playerPos = glm::vec2(
		this->Width / 2 - PLAYER_SIZE.x / 2,
		this->Height - PLAYER_SIZE.y
	);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

	// initialize ball
	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));

}

void Game::Update(GLfloat dt)
{
	// Update objects
	Ball->Move(dt, this->Width);
	// Handle collisions
	this->DoCollisions(dt);
	// update particle generator
	ParticleGen->Update(dt, *Ball, 2, glm::vec2(Ball->Radius * 0.5f));
	// Check loss condition
	if (Ball->Position.y >= this->Height)	// Did ball reach bottom edge
	{
		this->ResetLevel();
		this->ResetPlayer();
	}

}

void Game::ProcessInput(GLfloat dt)
{
	if (this->State == GAME_ACTIVE)
	{
		GLfloat velocity = PLAYER_VELOCITY * dt;
		// Move player paddle
		if (this->Keys[GLFW_KEY_A])
		{
			if (Player->Position.x >= 0)
			{
				Player->Position.x -= velocity;
				if (Ball->Stuck)
				{
					Ball->Position.x -= velocity;
				}
			}
		}
		if (this->Keys[GLFW_KEY_D])
		{
			if (Player->Position.x <= this->Width - Player->Size.x)
			{
				Player->Position.x += velocity;
				if (Ball->Stuck)
				{
					Ball->Position.x += velocity;
				}
			}
		}
		if (this->Keys[GLFW_KEY_SPACE])
		{
			Ball->Stuck = false;
		}
	}
}

void Game::Render()
{
	if (this->State == GAME_ACTIVE)
	{
		// Draw background
		Renderer->DrawSprite(ResourceManager::GetTexture("background"),
			glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f);
		// Draw level
		this->Levels[this->CurrentLevel].Draw(*Renderer);

		// draw player
		Player->Draw(*Renderer);
		// draw particlesd
		ParticleGen->Draw();
		// draw ball
		Ball->Draw(*Renderer);
	}
}

void Game::DoCollisions(GLfloat dt) 
{
	for (GameObject &box : this->Levels[this->CurrentLevel].Bricks)
	{
		if (!box.Destroyed)
		{
			Collision collision = CollisionHandler::CheckCollision(*Ball, box);
			if (std::get<0>(collision))		// collision true?
			{
				// Destroy block if not solid
				if (!box.IsSolid)
				{
					box.Destroyed = GL_TRUE;
				}
				// Collision resolution
				Direction dir = std::get<1>(collision);
				glm::vec2 diffVector = std::get<2>(collision);
				if (dir == LEFT || dir == RIGHT)	// Horizontal collision
				{
					Ball->Velocity.x = -Ball->Velocity.x;	// Reverse velocity
					// Relocate
					GLfloat penetration = Ball->Radius - std::abs(diffVector.x);
					if (dir == LEFT)
					{
						// Move ball to right
						Ball->Position.x += penetration;
					}
					else
					{
						// Move ball to left
						Ball->Position.x -= penetration;
					}
				}
				else								// Vertical collision
				{
					Ball->Velocity.y = -Ball->Velocity.y;
					// Relocate
					GLfloat penetration = Ball->Radius - std::abs(diffVector.y);
					if (dir == UP)
					{
						// Move ball back up
						Ball->Position.y -= penetration;
					}
					else
					{
						// Move ball down
						Ball->Position.y += penetration;
					}
				}
			}
		}
	}

	// Check collisions with ball and player paddle (unless stuck)
	Collision result = CollisionHandler::CheckCollision(*Ball, *Player);
	if (!Ball->Stuck && std::get<0>(result))
	{
		// Check where it hit the board, and change velocity based on where it hit the board
		GLfloat centerBoard = Player->Position.x + Player->Size.x * 0.5f;
		GLfloat distance = (Ball->Position.x + Ball->Radius) - centerBoard;
		GLfloat percentage = distance / (Player->Size.x * 0.5f);
		// Then move accordingly
		GLfloat strength = 2.0f;
		glm::vec2 oldVelocity = Ball->Velocity;
		Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
		Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
		// Fix sticky paddle ( assumes striking player paddle from top)
		Ball->Velocity.y = -1 * abs(Ball->Velocity.y);
	}
}

void Game::ResetLevel()
{
	std::string level = "levels/one.lvl";
	switch (this->CurrentLevel)
	{
	case 0:
		level = "levels/one.lvl";
		break;
	case 1:
		level = "levels/two.lvl";
		break;
	case 2:
		level = "levels/three.lvl";
		break;
	case 3:
		level = "levels/four.lvl";
		break;
	}

	this->Levels[this->CurrentLevel].Load(level.c_str(), this->Width, this->Height * 0.5f);
}

void Game::ResetPlayer()
{
	// Reset player/ball size
	Player->Size = PLAYER_SIZE;
	Player->Position = glm::vec2(this->Width * 0.5f - PLAYER_SIZE.x * 0.5f, this->Height - PLAYER_SIZE.y);
	Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x * 0.5f - BALL_RADIUS, -(BALL_RADIUS * 2)), INITIAL_BALL_VELOCITY);
}