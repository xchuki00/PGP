#include "controler.h"
#include <cstdio>
#include <iostream>
#include <SDL2\SDL.h>
float turn = 0.1f;
float move = 0.5f;
float x, y;
float rx = 0.0f, ry = 0.0f;
float hAngle;
float oldHAngle;
// vertical angle : 0, look at the horizon
float vAngle = 0.0f;
float oldVAngle = 0.0f;
bool controler::keyDown(SDL_Event const& e)
{
	glm::vec3 c;
	switch (e.key.keysym.sym) {
	case SDLK_ESCAPE:
		SDL_Quit();
		exit(0);
		break;
	case SDLK_w:	//move camera foward
		position += dir*move;
		break;
	case SDLK_s:	//move camera back
		position -= dir*move;
		break;
	case SDLK_a:	//move camera back
		c = glm::cross(dir, up);
		position -= c*move;
		break;
	case SDLK_d:	//move camera back
		c = glm::cross(dir, up);
		position += c*move;
		break;
	default:
		break;
	}

	Projection = glm::perspective(glm::half_pi<float>() / 2, 4.0f / 3.0f, 0.1f, 100.0f);
	View = glm::lookAt(position, position + dir, up);
	return true;
}

bool controler::mouseDown(SDL_Event const & e)
{
	if (e.button.button == SDL_BUTTON_LEFT) {
		std::cout << e.button.x<<","<<e.button.y<<std::endl;
		x = e.button.x;
		y = e.button.y;
		draging = true;
	}
	return true;
}

bool controler::mouseUp(SDL_Event const & e)
{
	if (e.button.button == SDL_BUTTON_LEFT) {
		std::cout << e.button.x << "," << e.button.y << std::endl;
		draging = false;
	}
	return true;
}

bool controler::MouseMotion(SDL_Event const & e)
{
	if (draging) {
		std::cout << e.motion.x << "," << e.motion.y << std::endl;
		if (e.motion.state & SDL_BUTTON_LMASK) {
			rx = e.motion.xrel;
			ry = e.motion.yrel;
		}
		//Projection = glm::perspective(glm::half_pi<float>() / 2, 4.0f / 3.0f, 0.1f, 100.0f);
		dir = glm::rotate(dir, -rx/20, up);
		glm::vec3 c = glm::cross(dir, up);
		dir = glm::rotate(dir, -ry / 20, c);
		//up = glm::rotate(up, ry / 20, c);
		View = glm::lookAt(position, position + dir, up);
	}
	return true;
}

void controler::setLightSource(LightSource * l)
{
	ls = l;
}

glm::mat4 controler::getView()
{
	return View;
}

glm::mat4 controler::getProjection()
{
	return this->Projection;
}

glm::vec3 controler::getPosition()
{
	return this->position;
}

glm::vec3 controler::getDir()
{
	return this->dir;
}

glm::vec3 controler::getUp()
{
	return this->up;
}

glm::vec3 controler::getDragedPosition()
{
	return this->dragedPosition;
}



controler::controler(std::shared_ptr<ge::ad::SDLWindow> w, std::shared_ptr<ge::ad::SDLMainLoop> m)
{
	this->window = w;
	this->mainLoop = m;
	this->position = glm::vec3(0, 2, 5);
	this->dir = glm::vec3(0, 0, -1);
	this->up = glm::vec3(0, 1, 0);
	this->Projection = glm::perspective(glm::half_pi<float>() / 2, 4.0f / 3.0f, 0.1f, 100000.0f);
	this->View = glm::lookAt(position, position + dir, up);
}


controler::~controler()
{
}


int controler::getWindowWidth()
{
	return window->getWidth();
}

int controler::getWindowHeight()
{
	return window->getHeight();
}
