/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include <time.h>
#include <iostream>
#include <string.h>
#include <irrKlang.h>

#include "Game.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"
#include "GameObject.h"
#include "PlayerObject.h"
#include "ObstacleObject.h"
#include "BackgroundObject.h"

//#include "GridObject.h"
// In game lives and ammo
GLint numLives = 3;
GLint ammo = 3;
int points = 0;

// Auxialiary
int indexBullets = 0;
bool bulletCooldown = false;
const int MAX_INDEX = 1000;
int spriteSpeed = 0;

using namespace irrklang;
using namespace std;

ISoundEngine* SoundEngine2 = createIrrKlangDevice();
bool once = true;

// Game-related State data
SpriteRenderer* Renderer;
SpriteRenderer* RendererPlayer;
SpriteRenderer* RendererBoost;

BackgroundObject* Background3;
BackgroundObject* Background3_2;
BackgroundObject* Background2;
BackgroundObject* Background2_2;
BackgroundObject* Ground1;
BackgroundObject* Ground1_2;

PlayerObject* Player;
PlayerObject* Boost;
PlayerObject* Bullets[MAX_INDEX];

ObstacleObject* Building1;
ObstacleObject* Building2;
ObstacleObject* Building3;
ObstacleObject* Building4;
ObstacleObject* Building5;
ObstacleObject* Building6;
ObstacleObject* Building7;
ObstacleObject* Building8;
ObstacleObject* Building9;
ObstacleObject* Building10;
ObstacleObject* Bird1;
ObstacleObject* Bird2;
ObstacleObject* Bird3;
ObstacleObject* Bird4;
ObstacleObject* Bird5;
ObstacleObject* BalaUp;
ObstacleObject* Cooldown;

GameObject* HealthBar;
GameObject* BulletCount;
GameObject* GameOver;
GameObject* StartGame;
GameObject* Tutorial;

Game::Game(GLuint width, GLuint height)
	: State(GAME_ACTIVE), Keys(), Width(width), Height(height), SpacePressed(false)
{

}

Game::~Game()
{
	delete Renderer;
	delete Player;
}

void Game::Init()
{
	// Load shaders
	ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.frag", nullptr, "sprite");

	// Configure shaders
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width), static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInteger("sprite", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
	// Load textures
	ResourceManager::LoadTexture("textures/coracao3.png", GL_TRUE, "HealthBar3");
	ResourceManager::LoadTexture("textures/coracao2.png", GL_TRUE, "HealthBar2");
	ResourceManager::LoadTexture("textures/coracao1.png", GL_TRUE, "HealthBar1");
	ResourceManager::LoadTexture("textures/Camada1.png", GL_TRUE, "ground");
	ResourceManager::LoadTexture("textures/Camada2.png", GL_TRUE, "background2");
	ResourceManager::LoadTexture("textures/Camada3.png", GL_TRUE, "background3");

	ResourceManager::LoadTexture("textures/Predio.png", GL_TRUE, "Predio");
	ResourceManager::LoadTexture("textures/Bird.png", GL_TRUE, "Bird");
	
	ResourceManager::LoadTexture("textures/BalaUp.png", GL_TRUE, "BalaUp");
	ResourceManager::LoadTexture("textures/Bala.png", GL_TRUE, "Bullet");
	ResourceManager::LoadTexture("textures/BalaContador10.png", GL_TRUE, "Bullet10");
	ResourceManager::LoadTexture("textures/BalaContador9.png", GL_TRUE, "Bullet9");
	ResourceManager::LoadTexture("textures/BalaContador8.png", GL_TRUE, "Bullet8");
	ResourceManager::LoadTexture("textures/BalaContador7.png", GL_TRUE, "Bullet7");
	ResourceManager::LoadTexture("textures/BalaContador6.png", GL_TRUE, "Bullet6");
	ResourceManager::LoadTexture("textures/BalaContador5.png", GL_TRUE, "Bullet5");
	ResourceManager::LoadTexture("textures/BalaContador4.png", GL_TRUE, "Bullet4");
	ResourceManager::LoadTexture("textures/BalaContador3.png", GL_TRUE, "Bullet3");
	ResourceManager::LoadTexture("textures/BalaContador2.png", GL_TRUE, "Bullet2");
	ResourceManager::LoadTexture("textures/BalaContador1.png", GL_TRUE, "Bullet1");
	ResourceManager::LoadTexture("textures/Branco.png", GL_TRUE, "Bullet0");
	ResourceManager::LoadTexture("textures/Plane.png", GL_TRUE, "Plane");
	ResourceManager::LoadTexture("textures/BoostSheet.png", GL_TRUE, "Boost");
	ResourceManager::LoadTexture("textures/GameOver.jpg", GL_TRUE, "GameOver");
	ResourceManager::LoadTexture("textures/Menu.jpg", GL_TRUE, "StartGame");
	ResourceManager::LoadTexture("textures/Tutorial.jpg", GL_TRUE, "Tutorial");

	ResourceManager::LoadTexture("textures/Verde.png", GL_TRUE, "Verde");
	ResourceManager::LoadTexture("textures/Vermelho.png", GL_TRUE, "Vermelho");

	// Set render-specific controls
	Shader shader1 = ResourceManager::GetShader("sprite");
	Renderer = new SpriteRenderer(shader1);

	// Player
	Player = new PlayerObject(glm::vec2(200, 300), glm::vec2(140, 45), ResourceManager::GetTexture("Plane"), glm::vec2(0.0f, 0.0f));
	Boost = new PlayerObject(glm::vec2(150, 322), glm::vec2(60, 24), ResourceManager::GetTexture("Boost"), glm::vec2(0.0f, 0.0f));

	// Bullets 10
	BulletCount = new GameObject(glm::vec2(0, 0), glm::vec2(this->Width, this->Height), ResourceManager::GetTexture("Bullet10"));
	// GameOver
	GameOver = new GameObject(glm::vec2(1280, 0), glm::vec2(this->Width, this->Height), ResourceManager::GetTexture("GameOver"));
	// StartGame
	StartGame = new GameObject(glm::vec2(0, 0), glm::vec2(this->Width, this->Height), ResourceManager::GetTexture("StartGame"));
	// Tutorial
	Tutorial = new GameObject(glm::vec2(0, 1280), glm::vec2(this->Width, this->Height), ResourceManager::GetTexture("Tutorial"));
	// HealthBar
	HealthBar = new GameObject(glm::vec2(0, 0), glm::vec2(800, 400), ResourceManager::GetTexture("HealthBar3"));

	// Background3
	Background3 = new BackgroundObject(glm::vec2(0, 0), glm::vec2(1280, this->Height), ResourceManager::GetTexture("background3"), glm::vec2 (0.5f, 0.0f));
	Background3_2 = new BackgroundObject(glm::vec2(1280, 0), glm::vec2((this->Width), (this->Height)), ResourceManager::GetTexture("background3"), glm::vec2(0.5f, 0.0f));

	// Background2
	Background2 = new BackgroundObject(glm::vec2(0, 0), glm::vec2(1280, this->Height), ResourceManager::GetTexture("background2"), glm::vec2(0.8f, 0.0f));
	Background2_2 = new BackgroundObject(glm::vec2(1280, 0), glm::vec2((this->Width), (this->Height)), ResourceManager::GetTexture("background2"), glm::vec2(0.8f, 0.0f));

	// Ground
	Ground1 = new BackgroundObject(glm::vec2(0, 636), glm::vec2(1280, 84), ResourceManager::GetTexture("ground"), glm::vec2(4.0f, 0.0f));
	Ground1_2 = new BackgroundObject(glm::vec2(1280, 636), glm::vec2(1280, 84), ResourceManager::GetTexture("ground"), glm::vec2(4.0f, 0.0f));

	// Predios
	Building1 = new ObstacleObject(glm::vec2(((this->Width / 10) * 5) - 32.5, 400), glm::vec2(65, 700), ResourceManager::GetTexture("Predio"));
	Building2 = new ObstacleObject(glm::vec2(((this->Width / 10) * 6) - 32.5, 600), glm::vec2(65, 700), ResourceManager::GetTexture("Predio"));
	Building3 = new ObstacleObject(glm::vec2(((this->Width / 10) * 8) - 32.5, 700), glm::vec2(65, 700), ResourceManager::GetTexture("Predio"));
	Building4 = new ObstacleObject(glm::vec2(((this->Width / 10) * 9) - 32.5, 200), glm::vec2(65, 700), ResourceManager::GetTexture("Predio"));
	Building5 = new ObstacleObject(glm::vec2(((this->Width / 10) * 10) - 32.5, 400), glm::vec2(65, 700), ResourceManager::GetTexture("Predio"));
	Building6 = new ObstacleObject(glm::vec2(((this->Width / 10) * 12) - 32.5, 600), glm::vec2(65, 700), ResourceManager::GetTexture("Predio"));
	Building7 = new ObstacleObject(glm::vec2(((this->Width / 10) * 13) - 32.5, 650), glm::vec2(65, 700), ResourceManager::GetTexture("Predio"));
	Building8 = new ObstacleObject(glm::vec2(((this->Width / 10) * 15) - 32.5, 200), glm::vec2(65, 700), ResourceManager::GetTexture("Predio"));
	Building9 = new ObstacleObject(glm::vec2(((this->Width / 10) * 17) - 32.5, 500), glm::vec2(65, 700), ResourceManager::GetTexture("Predio"));
	Building10 = new ObstacleObject(glm::vec2(((this->Width / 10) * 19) - 32.5, 600), glm::vec2(65, 700), ResourceManager::GetTexture("Predio"));

	// Passaros
	Bird1 = new ObstacleObject(glm::vec2(((this->Width / 10) * 9), 100), glm::vec2(50, 30), ResourceManager::GetTexture("Bird"));
	Bird2 = new ObstacleObject(glm::vec2(((this->Width / 10) * 17), 120), glm::vec2(50, 30), ResourceManager::GetTexture("Bird"));
	Bird3 = new ObstacleObject(glm::vec2(((this->Width / 10) * 7), 170), glm::vec2(50, 30), ResourceManager::GetTexture("Bird"));
	Bird4 = new ObstacleObject(glm::vec2(((this->Width / 10) * 22), 110), glm::vec2(50, 30), ResourceManager::GetTexture("Bird"));
	Bird5 = new ObstacleObject(glm::vec2(((this->Width / 10) * 15), 80), glm::vec2(50, 30), ResourceManager::GetTexture("Bird"));
	
	// Bala Up
	BalaUp = new ObstacleObject(glm::vec2(((this->Width / 10) * 11), 300), glm::vec2(20, 50), ResourceManager::GetTexture("BalaUp"));
}

void Game::Update(GLfloat dt)
{
	if (this->State == GAME_ACTIVE) {
		Background3->Move(Background3);
		Background3_2->Move(Background3_2);
		Background2->Move(Background2);
		Background2_2->Move(Background2_2);

		Ground1->Move(Ground1);
		Ground1_2->Move(Ground1_2);

		Building1->Move(Building1, false);
		Building2->Move(Building2, false);
		Building3->Move(Building3, false);
		Building4->Move(Building4, false);
		Building5->Move(Building5, false);
		Building6->Move(Building6, false);
		Building7->Move(Building7, false);
		Building8->Move(Building8, false);
		Building9->Move(Building9, false);
		Building10->Move(Building10, false);

		Bird1->Move(Bird1, true);
		Bird2->Move(Bird2, true);
		Bird3->Move(Bird3, true);
		Bird4->Move(Bird4, true);
		Bird5->Move(Bird5, true);

		BalaUp->Move(BalaUp, false);

		for (int i = 0; i < indexBullets; i++) {
			Bullets[i]->Move(Bullets[i]);
		}

		// Check for collisions
		this->DoCollisions();
	}
}

void Game::ProcessInput(GLfloat dt, bool cooldown)
{
	// Fast reset
	if (this->Keys[GLFW_KEY_R] and this->State == GAME_OVER) {
		this->Init();
		this->State = GAME_ACTIVE;
		numLives = 3;
		ammo = 3;
		indexBullets = 0;
		once = true;
		SoundEngine2->play2D("audio/press.mp3", false);
	}
	// Menu
	if (this->Keys[GLFW_KEY_ENTER] and this->State == GAME_OVER) {
		this->Init();
		StartGame->Position.x = 0.0f;
		this->State = GAME_MENU;
		SoundEngine2->play2D("audio/press.mp3", false);
	}
	// Start
	else if (this->Keys[GLFW_KEY_ENTER] and this->State == GAME_MENU) {
		this->Init();
		this->State = GAME_ACTIVE;
		numLives = 3;
		ammo = 3;
		indexBullets = 0;
		once = true;
		SoundEngine2->play2D("audio/press.mp3", false);
	}
	// Tutorial
	if (this->Keys[GLFW_KEY_P] and this->State == GAME_MENU) {
		Tutorial->Position.y = 0.0f;
		this->State = GAME_TUTORIAL;
		SoundEngine2->play2D("audio/press.mp3", false);
	}
	// Return Menu
	if (this->Keys[GLFW_KEY_BACKSPACE] and this->State == GAME_TUTORIAL) {
		Tutorial->Position.y = 1280.0f;
		StartGame->Position.x = 0.0f;
		this->State = GAME_MENU;
		SoundEngine2->play2D("audio/press.mp3", false);
	}

	// Game start
	if (this->State == GAME_ACTIVE)
	{
		GLfloat velocity = PLAYER_VELOCITY * dt;
		// Move playerboard
		
		StartGame->Position.x = 1280;
		Tutorial->Position.y = 1280;
		Player->Stuck = false;
		Boost->Stuck = false;
		Background3->Stuck = false;
		Background3_2->Stuck = false;
		Background2->Stuck = false;
		Background2_2->Stuck = false;
		Ground1->Stuck = false;
		Ground1_2->Stuck = false;
		Building1->Stuck = false;
		Building2->Stuck = false;
		Building3->Stuck = false;
		Building4->Stuck = false;
		Building5->Stuck = false;
		Building6->Stuck = false;
		Building7->Stuck = false;
		Building8->Stuck = false;
		Building9->Stuck = false;
		Building10->Stuck = false;
		Bird1->Stuck = false;
		Bird2->Stuck = false;
		Bird3->Stuck = false;
		Bird4->Stuck = false;
		Bird5->Stuck = false;
		BalaUp->Stuck = false;
		Cooldown->Stuck = false;

		if (!Player->Stuck) {
			if (this->Keys[GLFW_KEY_W])
			{
				if (Player->Position.y >= 0) {
					Player->Position.y -= velocity;
					Boost->Position.y -= velocity;
				}
			}
			if (this->Keys[GLFW_KEY_S])
			{
				if (Player->Position.y <= this->Height - 120.0f) {
					Player->Position.y += velocity;
					Boost->Position.y += velocity;
				}
			}
			if (this->SpacePressed and !cooldown)
			{
				glm::vec2 bulletPos = glm::vec2(320, 320);
				PlayerObject *Bullet = new PlayerObject(bulletPos, glm::vec2(50, 20), ResourceManager::GetTexture("Bullet"), glm::vec2(12.0f, 0.0f));

				Bullet->Stuck = false;
				Bullet->Position.x = Player->Position.x + 120.0f;
				Bullet->Position.y = Player->Position.y + 20.0f;

				if (ammo == 0)
					SoundEngine2->play2D("audio/solid.mp3", false);
				else
					SoundEngine2->play2D("audio/bullet.mp3", false);
				if (indexBullets < MAX_INDEX and ammo > 0) {
					Bullets[indexBullets++] = Bullet;
					ammo -= 1;
				}
				else if (indexBullets == MAX_INDEX)
					cout << "Maximo de municao usada no jogo inteiro! - " << indexBullets << endl;
				this->SpacePressed = false;
			}
		}
	}
	// Cooldown
	if (cooldown) {
		Cooldown = new ObstacleObject(glm::vec2(20, 650), glm::vec2(50, 50), ResourceManager::GetTexture("Vermelho"));
	}
	else {
		Cooldown = new ObstacleObject(glm::vec2(20, 650), glm::vec2(50, 50), ResourceManager::GetTexture("Verde"));
	}
}

void Game::Render()
{
	if (this->State == GAME_MENU) { StartGame->Draw(*Renderer, 0.02f); }
	if (this->State == GAME_TUTORIAL) { Tutorial->Draw(*Renderer, 0.02f); }
	if (this->State == GAME_OVER) { GameOver->Draw(*Renderer, 0.02f); }
	if (this->State == GAME_ACTIVE)
	{
		// Player sprite init
		Shader shader1 = ResourceManager::GetShader("sprite");
		RendererPlayer = new SpriteRenderer(shader1, Player->TexturePlayerPosX, Player->TexturePlayerPosX + 1.0f, false);
		
		if (Boost->TextureBoostPosX >= 3.0f / 4.0f) { Boost->TextureBoostPosX = 0.0f; Boost->TextureBoostPosY = 1.0f / 4.0f; }
		if (spriteSpeed % 10 == 0)
			RendererBoost = new SpriteRenderer(shader1, Boost->TextureBoostPosX += 1.0f / 4.0f, Boost->TextureBoostPosY += 1.0f / 4.0f, true);
		else
			RendererBoost = new SpriteRenderer(shader1, Boost->TextureBoostPosX, Boost->TextureBoostPosY, true);

		// Background
		Background3->Draw(*Renderer, 0.2f);
		Background3_2->Draw(*Renderer, 0.2f);
		Background2->Draw(*Renderer, 0.1f);
		Background2_2->Draw(*Renderer, 0.1f);

		// Ammo
		if (ammo == 10) { BulletCount = new GameObject(glm::vec2(0, 0), glm::vec2(this->Width, this->Height), ResourceManager::GetTexture("Bullet10")); }
		else if (ammo == 9) { BulletCount = new GameObject(glm::vec2(0, 0), glm::vec2(this->Width, this->Height), ResourceManager::GetTexture("Bullet9")); }
		else if (ammo == 8) { BulletCount = new GameObject(glm::vec2(0, 0), glm::vec2(this->Width, this->Height), ResourceManager::GetTexture("Bullet8")); }
		else if (ammo == 7) { BulletCount = new GameObject(glm::vec2(0, 0), glm::vec2(this->Width, this->Height), ResourceManager::GetTexture("Bullet7")); }
		else if (ammo == 6) { BulletCount = new GameObject(glm::vec2(0, 0), glm::vec2(this->Width, this->Height), ResourceManager::GetTexture("Bullet6")); }
		else if (ammo == 5) { BulletCount = new GameObject(glm::vec2(0, 0), glm::vec2(this->Width, this->Height), ResourceManager::GetTexture("Bullet5")); }
		else if (ammo == 4) { BulletCount = new GameObject(glm::vec2(0, 0), glm::vec2(this->Width, this->Height), ResourceManager::GetTexture("Bullet4")); }
		else if (ammo == 3) { BulletCount = new GameObject(glm::vec2(0, 0), glm::vec2(this->Width, this->Height), ResourceManager::GetTexture("Bullet3")); }
		else if (ammo == 2) { BulletCount = new GameObject(glm::vec2(0, 0), glm::vec2(this->Width, this->Height), ResourceManager::GetTexture("Bullet2")); }
		else { BulletCount = new GameObject(glm::vec2(0, 0), glm::vec2(this->Width, this->Height), ResourceManager::GetTexture("Bullet1")); }
		
		if (ammo != 0) { BulletCount->Draw(*Renderer, 0.2f); }

		// Health Bar
		if (numLives == 3) { HealthBar->Draw(*Renderer, 0.2f); }
		else if (numLives == 2) { HealthBar = new GameObject(glm::vec2(0.0f, 0.0f), glm::vec2(800, 400), ResourceManager::GetTexture("HealthBar2")); }
		else if (numLives == 1) { HealthBar = new GameObject(glm::vec2(0.0f, 0.0f), glm::vec2(800, 400), ResourceManager::GetTexture("HealthBar1")); }
		else {
			if (once) {
				SoundEngine2->play2D("audio/explosion.mp3", false);
				once = false;
			}
			GameOver->Position.x = 0.0f;
			this->State = GAME_OVER;
			indexBullets = 0;
			cout << "Pontuacao Total: " << points << endl;
		}
		HealthBar->Draw(*Renderer, 0.2f);

		// Building
		if (!Building1->Destroyed) { Building1->Draw(*Renderer, 0.02f); }
		if (!Building2->Destroyed) { Building2->Draw(*Renderer, 0.02f); }
		if (!Building3->Destroyed) { Building3->Draw(*Renderer, 0.02f); }
		if (!Building4->Destroyed) { Building4->Draw(*Renderer, 0.02f); }
		if (!Building5->Destroyed) { Building5->Draw(*Renderer, 0.02f); }
		if (!Building6->Destroyed) { Building6->Draw(*Renderer, 0.02f); }
		if (!Building7->Destroyed) { Building7->Draw(*Renderer, 0.02f); }
		if (!Building8->Destroyed) { Building8->Draw(*Renderer, 0.02f); }
		if (!Building9->Destroyed) { Building9->Draw(*Renderer, 0.02f); }
		if (!Building10->Destroyed) { Building10->Draw(*Renderer, 0.02f); }

		// Ground
		Ground1->Draw(*Renderer, 0.02f);
		Ground1_2->Draw(*Renderer, 0.02f);

		// Cooldown
		Cooldown->Draw(*Renderer, 0.2f);

		// Birds
		if (!Bird1->Destroyed) { Bird1->Draw(*Renderer, 0.02f); }
		if (!Bird2->Destroyed) { Bird2->Draw(*Renderer, 0.02f); }
		if (!Bird3->Destroyed) { Bird3->Draw(*Renderer, 0.02f); }
		if (!Bird4->Destroyed) { Bird4->Draw(*Renderer, 0.02f); }
		if (!Bird5->Destroyed) { Bird5->Draw(*Renderer, 0.02f); }

		// Bala Up
		if (!BalaUp->Destroyed) { BalaUp->Draw(*Renderer, 0.02f); }

		// Player
		Player->DrawPlayer(*RendererPlayer, 0.2f);
		Boost->DrawPlayer(*RendererBoost, 0.2f);
		
		// Bullets
		for (int i = 0; i < indexBullets; i++) {
			if (Bullets[i]->Position.x <= 1400.0f and !Bullets[i]->Destroyed)
				Bullets[i]->Draw(*Renderer, 0.2f);
		}

		// Control boost sprite speed
		spriteSpeed++;
	}
}


void Game::DoCollisions()
{
	if (CheckCollision(*Player, *Building1))
	{
		if (!Building1->Destroyed) {
			Building1->Destroyed = true;
			numLives -= 1;
			SoundEngine2->play2D("audio/health.mp3", false);
		}
	}
	else if (CheckCollision(*Player, *Building2))
	{
		if (!Building2->Destroyed) {
			Building2->Destroyed = true;
			numLives -= 1;
			SoundEngine2->play2D("audio/health.mp3", false);
		}
	}
	else if (CheckCollision(*Player, *Building3))
	{
		if (!Building3->Destroyed) {
			Building3->Destroyed = true;
			numLives -= 1;
			SoundEngine2->play2D("audio/health.mp3", false);
		}
	}
	else if (CheckCollision(*Player, *Building4))
	{
		if (!Building4->Destroyed) {
			Building4->Destroyed = true;
			numLives -= 1;
			SoundEngine2->play2D("audio/health.mp3", false);
		}
	}
	else if (CheckCollision(*Player, *Building5))
	{
		if (!Building5->Destroyed) {
			Building5->Destroyed = true;
			numLives -= 1;
			SoundEngine2->play2D("audio/health.mp3", false);
		}
	}
	else if (CheckCollision(*Player, *Building6))
	{
		if (!Building6->Destroyed) {
			Building6->Destroyed = true;
			numLives -= 1;
			SoundEngine2->play2D("audio/health.mp3", false);
		}
	}
	else if (CheckCollision(*Player, *Building7))
	{
		if (!Building7->Destroyed) {
			Building7->Destroyed = true;
			numLives -= 1;
			SoundEngine2->play2D("audio/health.mp3", false);
		}
	}
	else if (CheckCollision(*Player, *Building8))
	{
		if (!Building8->Destroyed) {
			Building8->Destroyed = true;
			numLives -= 1;
			SoundEngine2->play2D("audio/health.mp3", false);
		}
	}
	else if (CheckCollision(*Player, *Building9))
	{
		if (!Building9->Destroyed) {
			Building9->Destroyed = true;
			numLives -= 1;
			SoundEngine2->play2D("audio/health.mp3", false);
		}
	}
	else if (CheckCollision(*Player, *Building10))
	{
		if (!Building10->Destroyed) {
			Building10->Destroyed = true;
			numLives -= 1;
			SoundEngine2->play2D("audio/health.mp3", false);
		}
	}
	// Bird
	else if (CheckCollision(*Player, *Bird1))
	{
		if (!Bird1->Destroyed) {
			Bird1->Destroyed = true;
			numLives -= 1;
			SoundEngine2->play2D("audio/health.mp3", false);
		}
	}
	else if (CheckCollision(*Player, *Bird2))
	{
		if (!Bird2->Destroyed) {
			Bird2->Destroyed = true;
			numLives -= 1;
			SoundEngine2->play2D("audio/health.mp3", false);
		}
	}
	else if (CheckCollision(*Player, *Bird3))
	{
		if (!Bird3->Destroyed) {
			Bird3->Destroyed = true;
			numLives -= 1;
			SoundEngine2->play2D("audio/health.mp3", false);
		}
	}
	else if (CheckCollision(*Player, *Bird4))
	{
		if (!Bird4->Destroyed) {
			Bird4->Destroyed = true;
			numLives -= 1;
			SoundEngine2->play2D("audio/health.mp3", false);
		}
	}
	else if (CheckCollision(*Player, *Bird5))
	{
		if (!Bird5->Destroyed) {
			Bird5->Destroyed = true;
			numLives -= 1;
			SoundEngine2->play2D("audio/health.mp3", false);
		}
	}
	// Bala Up
	else if (CheckCollision(*Player, *BalaUp))
	{
		if (!BalaUp->Destroyed) {
			BalaUp->Destroyed = true;
			if (ammo < 10)
				ammo += 1;
			SoundEngine2->play2D("audio/powerup.mp3", false);
			points += 5;
		}
	}
	// Bullets
	for (int i = 0; i < indexBullets; i++) {
		if (CheckCollision(*Building1, *Bullets[i])) {
			if (!Building1->Destroyed and !Bullets[i]->Destroyed) {
				Building1->Destroyed = true;
				Bullets[i]->Destroyed = true;
				SoundEngine2->play2D("audio/destroy.mp3", false);
				points += 5;
			}
		}
		else if (CheckCollision(*Building2, *Bullets[i])) {
			if (!Building2->Destroyed and !Bullets[i]->Destroyed) {
				Building2->Destroyed = true;
				Bullets[i]->Destroyed = true;
				SoundEngine2->play2D("audio/destroy.mp3", false);
				points += 5;
			}
		}
		else if (CheckCollision(*Building3, *Bullets[i])) {
			if (!Building3->Destroyed and !Bullets[i]->Destroyed) {
				Building3->Destroyed = true;
				Bullets[i]->Destroyed = true;
				SoundEngine2->play2D("audio/destroy.mp3", false);
				points += 5;
			}
		}
		else if (CheckCollision(*Building4, *Bullets[i])) {
			if (!Building4->Destroyed and !Bullets[i]->Destroyed) {
				Building4->Destroyed = true;
				Bullets[i]->Destroyed = true;
				SoundEngine2->play2D("audio/destroy.mp3", false);
				points += 5;
			}
		}
		else if (CheckCollision(*Building5, *Bullets[i])) {
			if (!Building5->Destroyed and !Bullets[i]->Destroyed) {
				Building5->Destroyed = true;
				Bullets[i]->Destroyed = true;
				SoundEngine2->play2D("audio/destroy.mp3", false);
				points += 5;
			}
		}
		else if (CheckCollision(*Building6, *Bullets[i])) {
			if (!Building6->Destroyed and !Bullets[i]->Destroyed) {
				Building6->Destroyed = true;
				Bullets[i]->Destroyed = true;
				SoundEngine2->play2D("audio/destroy.mp3", false);
				points += 5;
			}
		}
		else if (CheckCollision(*Building7, *Bullets[i])) {
			if (!Building7->Destroyed and !Bullets[i]->Destroyed) {
				Building7->Destroyed = true;
				Bullets[i]->Destroyed = true;
				SoundEngine2->play2D("audio/destroy.mp3", false);
				points += 5;
			}
		}
		else if (CheckCollision(*Building8, *Bullets[i])) {
			if (!Building8->Destroyed and !Bullets[i]->Destroyed) {
				Building8->Destroyed = true;
				Bullets[i]->Destroyed = true;
				SoundEngine2->play2D("audio/destroy.mp3", false);
				points += 5;
			}
		}
		else if (CheckCollision(*Building9, *Bullets[i])) {
			if (!Building9->Destroyed and !Bullets[i]->Destroyed) {
				Building9->Destroyed = true;
				Bullets[i]->Destroyed = true;
				SoundEngine2->play2D("audio/destroy.mp3", false);
				points += 5;
			}
		}
		else if (CheckCollision(*Building10, *Bullets[i])) {
			if (!Building10->Destroyed and !Bullets[i]->Destroyed) {
				Building10->Destroyed = true;
				Bullets[i]->Destroyed = true;
				SoundEngine2->play2D("audio/destroy.mp3", false);
				points += 5;
			}
		}
		// Birds
		else if (CheckCollision(*Bird1, *Bullets[i])) {
			if (!Bird1->Destroyed and !Bullets[i]->Destroyed) {
				Bird1->Destroyed = true;
				Bullets[i]->Destroyed = true;
				SoundEngine2->play2D("audio/destroy.mp3", false);
				points += 15;
			}
		}
		else if (CheckCollision(*Bird2, *Bullets[i])) {
			if (!Bird2->Destroyed and !Bullets[i]->Destroyed) {
				Bird2->Destroyed = true;
				Bullets[i]->Destroyed = true;
				SoundEngine2->play2D("audio/destroy.mp3", false);
				points += 15;
			}
		}
		else if (CheckCollision(*Bird3, *Bullets[i])) {
			if (!Bird3->Destroyed and !Bullets[i]->Destroyed) {
				Bird3->Destroyed = true;
				Bullets[i]->Destroyed = true;
				SoundEngine2->play2D("audio/destroy.mp3", false);
				points += 15;
			}
		}
		else if (CheckCollision(*Bird4, *Bullets[i])) {
			if (!Bird4->Destroyed and !Bullets[i]->Destroyed) {
				Bird4->Destroyed = true;
				Bullets[i]->Destroyed = true;
				SoundEngine2->play2D("audio/destroy.mp3", false);
				points += 15;
			}
		}
		else if (CheckCollision(*Bird5, *Bullets[i])) {
			if (!Bird5->Destroyed and !Bullets[i]->Destroyed) {
				Bird5->Destroyed = true;
				Bullets[i]->Destroyed = true;
				SoundEngine2->play2D("audio/destroy.mp3", false);
				points += 15;
			}
		}
	}
}

GLboolean Game::CheckCollision(GameObject &one, GameObject &two) // AABB - AABB collision
{
	// Collision x-axis?
	GLboolean collisionX = one.Position.x + one.Size.x >= two.Position.x &&
		two.Position.x + two.Size.x >= one.Position.x;
	// Collision y-axis?
	GLboolean collisionY = one.Position.y + one.Size.y >= two.Position.y &&
		two.Position.y + two.Size.y >= one.Position.y;
	// Collision only if on both axes
	return collisionX && collisionY;
}