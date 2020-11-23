#pragma once
#include <SDL2\SDL.h>
#include <GPUEngine/geGL/StaticCalls.h>
#include <GPUEngine/geGL/geGL.h>
#include <GPUEngine/geAd/SDLWindow/SDLWindow.h>
#include <GPUEngine/geAd/SDLWindow/SDLMainLoop.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtc/matrix_access.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>
#include"LightSource.h"
class controler
{
	std::shared_ptr<ge::ad::SDLWindow> window;
	std::shared_ptr<ge::ad::SDLMainLoop> mainLoop;
	LightSource *ls;
	glm::mat4 View;
	glm::mat4 Projection;
	glm::vec3 position;
	glm::vec3 dir;
	glm::vec3 up;
	glm::vec3 right;
	bool draging = false;
	glm::vec3 dragedPosition = glm::vec3(1.0f);
public:

	controler(std::shared_ptr<ge::ad::SDLWindow> w, std::shared_ptr<ge::ad::SDLMainLoop> m);
	~controler();

	int getWindowWidth();
	int getWindowHeight();
	bool keyDown(SDL_Event const& e);
	bool mouseDown(SDL_Event const& e);
	bool mouseUp(SDL_Event const& e);
	bool MouseMotion(SDL_Event const& e);
	void setLightSource(LightSource *l);
	glm::mat4 getView();
	glm::mat4 getProjection();
	glm::vec3 getPosition();
	glm::vec3 getDir();
	glm::vec3 getUp();
	glm::vec3 getDragedPosition();
};

