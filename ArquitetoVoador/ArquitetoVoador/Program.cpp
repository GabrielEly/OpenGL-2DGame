/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Game.h"
#include "ResourceManager.h"

#include <irrKlang.h>
#include <time.h>
#include <iostream>
#include <string.h>

using namespace irrklang;
using namespace std;

bool notPressed = true;
bool onceGameOver = true;
bool ready = true;

// GLFW function declerations
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// The Width of the screen
const GLuint SCREEN_WIDTH = 1280;
// The height of the screen
const GLuint SCREEN_HEIGHT = 720;

Game ArquitetoVoador(SCREEN_WIDTH, SCREEN_HEIGHT);
ISoundEngine* SoundEngine = createIrrKlangDevice();

int main(int argc, char *argv[])
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Arquiteto Voador", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();
	glGetError(); // Call it once to catch glewInit() bug, all other errors are now from our application.

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);

	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;
	cout << endl;
	cout << "ARQUITETO VOADOR - Simulador de voo" << endl;
	cout << "Versao 1.1 - Aplha" << endl;

	// OpenGL configuration
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_CULL_FACE);
	//Ativando transparencia
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Initialize game
	ArquitetoVoador.Init();

	// DeltaTime variables
	GLfloat deltaTime = 0.0f;
	GLfloat lastFrame = 0.0f;

	// Start Game within Menu State
	ArquitetoVoador.State = GAME_MENU;

	SoundEngine->play2D("audio/breakout.mp3", true);
	
	int cooldown = 0;
	int tick = 0;
	
	while (!glfwWindowShouldClose(window))
	{
		if (ArquitetoVoador.State == GAME_OVER and onceGameOver) {
			SoundEngine->stopAllSounds();
			SoundEngine->play2D("audio/gameOver.mp3", true);
			onceGameOver = false;
		}

		// Calculate delta time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents();

		//deltaTime = 0.001f;
		// Manage user input
		// Pequeno delay para atirar e nao conseguir segurar espaço - muitas balas atirando ao mesmo tempo
		if (cooldown % 80 == 0) {
			ready = true;
		}
		if (ready and tick == 0) {
			if (!notPressed) {
				tick == 1;
				ready = false;
				notPressed = true;
			}
			ArquitetoVoador.ProcessInput(deltaTime, false);
		}
		else if (tick == 1) {
			tick == 0;
			ArquitetoVoador.ProcessInput(deltaTime, true);
		}
		else {
			ArquitetoVoador.ProcessInput(deltaTime, true);
		}
		cooldown++;

		// Update Game state
		ArquitetoVoador.Update(deltaTime);

		// Render
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		ArquitetoVoador.Render();

		glfwSwapBuffers(window);
	}

	// Delete all resources as loaded using the resource manager
	ResourceManager::Clear();

	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// When a user presses the escape key, we set the WindowShouldClose property to true, closing the application
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (key == GLFW_KEY_SPACE and action == GLFW_PRESS and ArquitetoVoador.State == GAME_ACTIVE and ready) {
			ArquitetoVoador.SpacePressed = true;
			notPressed = false;
		}
		else if (key == GLFW_KEY_R and action == GLFW_PRESS and ArquitetoVoador.State == GAME_OVER) {
			SoundEngine->stopAllSounds();
			SoundEngine->play2D("audio/breakout.mp3", true);
			ArquitetoVoador.Keys[key] = GL_TRUE;
			onceGameOver = true;
		}
		else if (key == GLFW_KEY_ENTER and action == GLFW_PRESS and ArquitetoVoador.State == GAME_OVER) {
			SoundEngine->stopAllSounds();
			SoundEngine->play2D("audio/breakout.mp3", true);
			ArquitetoVoador.Keys[key] = GL_TRUE;
			onceGameOver = true;
		}
		else if (action == GLFW_PRESS)
			ArquitetoVoador.Keys[key] = GL_TRUE;
		else if (action == GLFW_RELEASE)
			ArquitetoVoador.Keys[key] = GL_FALSE;
		else if (action == GLFW_REPEAT)
			ArquitetoVoador.Keys[key] = GL_TRUE;
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

#pragma once