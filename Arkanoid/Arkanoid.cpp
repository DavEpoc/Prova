#pragma once
#define _CRT_SECURE_NO_WARNINGS // to make console working in debug

//#include "windows.h"
#include<iostream>
#include <map>

#define STB_IMAGE_IMPLEMENTATION  // texture parcing data
#include"stb.h"
#include"stb_image.h"

#include<GL/glew.h>
#include<GL/GL.h>

#include<glm/mat4x4.hpp>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>
#include <math.h>

#include "SDL.h"

// all of the following should be into some bigger gameManagere structure
// but this game is simple
#define BRICKNUM 20
#define COLUMNSNUM 10

#define BRICKWIDTH 80
#define BRICKHEIGHT 20

#define BOARDDISTANCE 20

#define SDL_TIMEREVENT SDL_USEREVENT

/* global-variables */
bool quitGame = false;				   // End loop condition
SDL_Window *window;                    // Declare a pointer
SDL_Renderer *renderer;				   // Use renderer variable

const int FPS = 30;
SDL_GLContext glcontext;

float windowWidth  = static_cast<float>( (COLUMNSNUM + 2) * BRICKWIDTH );
float windowHeight = static_cast<float>( (BRICKNUM / COLUMNSNUM + 1) * BRICKHEIGHT + BOARDDISTANCE * BRICKHEIGHT );
float brickWidth   = static_cast<float>( BRICKWIDTH );
float brickHeight  = static_cast<float>( BRICKHEIGHT );

//mouse
float mouseXrelPos = 0.0f;

#include "Shader.h"
#include "Colors.h"
#include "Camera.h"

#include "Mesh.h"
#include "PhysicObjects.h"

#include "Ball.h"
#include "Brick.h"
#include "Board.h"
#include "BrickPool.h"
#include "Scene.h"

/* openGL properties */

bool setOpenGLAttributes()
{
	//mouse
	SDL_SetWindowGrab(window, SDL_TRUE);
	SDL_SetRelativeMouseMode(SDL_TRUE);

	// Set our OpenGL version.
	// SDL_GL_CONTEXT_CORE gives us only the newer version, deprecated functions are disabled
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);  //COMPATIBILITY

																							 // 3.2 is part of the modern versions of OpenGL, but most video cards whould be able to run it
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	// Turn on double buffering with a 24bit Z buffer.
	// You may need to change this to 16 or 32 for your system
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16); //set to 16

	return true;
}

void initGlewAndRandom()
{
	glewInit();
	srand(time(NULL));

}

bool initializeGameWindow()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)		// Initialize SDL2
	{
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return false;
	}


	// Create an application window with the following settings:
	window = SDL_CreateWindow(
		"An SDL2 window",							// window title
		SDL_WINDOWPOS_CENTERED,						// initial x position
		SDL_WINDOWPOS_CENTERED,						// initial y position
		windowWidth,				                // width, in pixels
		windowHeight,								// height, in pixels
		SDL_WINDOW_OPENGL							// using OpenGL
	);

	//Check that the window was successfully created
	if (window == NULL) {
		//In the case that the window could not be made...

		printf("Could not create window: %s\n", SDL_GetError());

		SDL_Quit();
		return false;
	}

	setOpenGLAttributes();

	//SDL with openGL needs Context
	glcontext = SDL_GL_CreateContext(window);

	// This makes our buffer swap syncronized with the monitor's vertical refresh
	SDL_GL_SetSwapInterval(1);

	glewExperimental = GL_TRUE;
	//initialize glew and rand() levels on r press
	initGlewAndRandom();


	//openGLOptions
	// Enable blending so that we can have transparanet object
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Enable depth testing so that closer triangles will hide the triangles farther away
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	return true;
}

/* input_manager.cpp */

unsigned int pushTimerEvent(unsigned int /*time*/, void* /*data*/) {
	SDL_Event e;
	e.type = SDL_TIMEREVENT;
	SDL_PushEvent(&e);
	return 1000 / FPS;
}

void callbackKeyboard(SDL_Event &e, bool isDown, Scene *scene)
{
	int key = e.key.keysym.sym;
	switch (key) {
	case SDLK_ESCAPE:
		quitGame = true;
		break;
	case SDLK_r:
		if (!isDown)
		{

			unsigned int randLevel = scene->findRandomLevel();
			//std::cout << randLevel << std::endl;
			if (randLevel != scene->getLevelNumber())
			{
				scene->initNewLevel(randLevel);
			}
			else
			{
				scene->initSameLevel();
			}
		}
		break;
	}
}

void callbackTimer(SDL_Event&, Scene *scene)
{
	scene->winningLevel();
	scene->doPhysicStep();  //change mvp
	glViewport(0, 0, windowWidth, windowHeight);

	glClearColor(0, 0, 0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	scene->render();

	SDL_GL_SwapWindow(window);
}

void processEvent(SDL_Event &e, Scene *scene) {
	switch (e.type) {
	case SDL_QUIT: quitGame = true; break;

	case SDL_KEYDOWN: callbackKeyboard(e, true, scene); break;
	case SDL_KEYUP: callbackKeyboard(e, false, scene); break;

	case SDL_MOUSEMOTION:
		mouseXrelPos = e.motion.xrel; break;

	case SDL_TIMEREVENT: callbackTimer(e, scene); break;
	}
}

/* END input_manager.cpp */

/* CleanUp */

void cleanUp(Scene &scene)
{
	// Cleanup all the things we bound and allocated 
	scene.clearShader();

	delete &scene;

	// Delete our OpengL context
	SDL_GL_DeleteContext(glcontext);

	// Destroy our WINDOW
	SDL_DestroyWindow(window);

	// Shutdown SDL 2
	SDL_Quit();
}

int main(int argc, char* args[])
{
	Scene *scene = new Scene();

	if (!initializeGameWindow())                       // initialize gamewindow
	{
		return -1;
	}

	/*							                       // uncomment console for debug
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	*/
	
	scene->initShader();							   // initialize shaders
		

	SDL_AddTimer(1000 / FPS, pushTimerEvent, NULL);    // Add timer to SDL events

	scene->initNewLevel();							   // initialize Game

	while (!quitGame) {								   // SDL event Loop
		SDL_Event e;
		SDL_WaitEvent(&e);
		processEvent(e,scene);
	}

	// Clean up
	cleanUp(*scene);											// CleanUp All
	return 0;
}

