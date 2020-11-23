#pragma once
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <GPUEngine/geGL/StaticCalls.h>
#include <GPUEngine/geGL/geGL.h>
#include <GPUEngine/geAd/SDLWindow/SDLWindow.h>
#include <GPUEngine/geAd/SDLWindow/SDLMainLoop.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtc/matrix_access.hpp>

#include "controler.h"
#include "Model.h"
#include "BaseShaders.h"
#include "LightSource.h"
#include "Models.h"
#include "sphere.h"
#include "water.h"
controler* c;
Model* sp;
Water* w;
bool HittedSphere(SDL_Event const& e) {

	return false;
}
bool keyDown(SDL_Event const& e) {
	if (e.key.keysym.sym == SDLK_1) {
		w->pushToPoint(0);
	}
	if (e.key.keysym.sym == SDLK_2) {
		w->pushToPoint(w->getCountofSurfacePoints()*0.85);
	}
	if (e.key.keysym.sym == SDLK_3) {
		w->pushToPoint(w->getCountofSurfacePoints()*0.6);
	}
	if (e.key.keysym.sym == SDLK_4) {
		w->pushToPoint(w->getCountofSurfacePoints()*0.55);
	}
	if (e.key.keysym.sym == SDLK_5) {
		w->pushToPoint(w->getCountofSurfacePoints()/4);
	}
	if (e.key.keysym.sym == SDLK_6) {
		w->pushToPoint(w->getCountofSurfacePoints()/8);
	}
	return c->keyDown(e);
	
}
bool mouseDown(SDL_Event const& e) {
	if (HittedSphere(e)) {
		//sp->setDraging();

	}
	return c->mouseDown(e);


}
bool mouseUp(SDL_Event const& e) {
	return c->mouseUp(e);
}
bool mouseMotion(SDL_Event const& e) {
	return c->MouseMotion(e);
}
bool windowResize(SDL_Event const& e) {
	std::cout << "DWADWA";
	w->resizeTexture();
	ge::gl::glViewport(0,0,c->getWindowWidth(),c->getWindowHeight());
	return true;
}

using namespace ge::gl;

int main(int, char*[]) {
	//create window
	auto mainLoop = std::make_shared<ge::ad::SDLMainLoop>();
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	auto window = std::make_shared<ge::ad::SDLWindow>();
	window->createContext("rendering");
	mainLoop->addWindow("mainWindow", window);
	//init OpenGL
	ge::gl::init(SDL_GL_GetProcAddress);
	ge::gl::setHighDebugMessage();
	//sphere = new Model();
	c= new controler(window,mainLoop);
	window->setEventCallback(SDL_KEYDOWN, keyDown);
	window->setEventCallback(SDL_MOUSEBUTTONDOWN, mouseDown);
	window->setEventCallback(SDL_MOUSEBUTTONUP, mouseUp);
	window->setEventCallback(SDL_MOUSEMOTION, mouseMotion);
	window->setWindowEventCallback(SDL_WINDOWEVENT_SIZE_CHANGED, windowResize);
	LightSource* ls = new LightSource(glm::vec3(-5,-10,-5),glm::vec3(1,1,1),1,1);
	c->setLightSource(ls);
	Model* aq = new Model();
	aq->setControler(c);
	sp = new Model();
	sp->setControler(c);
	glm::vec3 v[4] = {
		glm::vec3(-14.999f,0.0f,4.999f),
		glm::vec3(-14.999f,0.0f,-4.999f),
		glm::vec3(14.999f,0.0f,4.999f)
	};
	w = new Water(v,3,glm::vec3(0,1,0),300,100,4.999);
	w->setControler(c);
	w->setLightSource(ls);
	w->compileShaders();
	w->buffer();
	auto vs = std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER, BaseVSSrc);
	auto fs = std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER, BaseFSSrc);
	auto prgm = std::make_shared<ge::gl::Program>(vs, fs);
	auto upOrDown = prgm->getUniformLocation("upOrDown");
	//buffer data

	aq->buffer(prgm,AquariumVer,NULL,AquariumEl,AquariumCountOfVer,AquariumCountOfEl);
	aq->setTexture("../imgs/text.bmp", GL_BGR);

	sp->buffer(prgm,NULL, sphereVer,sphereEl, sphereCountOfVer,sphereCountOfEl);
	sp->setPosition(glm::translate(glm::mat4(1.0f), glm::vec3(10, 3, -1.5)));
	ls->glGetUniformLocationLight(prgm);
	w->addModel(sp);
	w->addModel(aq);
	Model* sp2 = new Model();
	sp2->setControler(c);
	sp2->setPosition(glm::translate(glm::mat4(1.0f), glm::vec3(-8, 3, 1)));
	sp2->buffer(prgm, NULL, sphereVer, sphereEl, sphereCountOfVer, sphereCountOfEl);
	Model* sp3 = new Model();
	sp3->setControler(c);
	sp3->setPosition(glm::translate(glm::mat4(1.0f), glm::vec3(0, -3, 2.697)));
	sp3->buffer(prgm, NULL, sphereVer, sphereEl, sphereCountOfVer, sphereCountOfEl);
	Model* pl = new Model();
	pl->setControler(c);
	//pl->setPosition(glm::translate(glm::mat4(1.0f), glm::vec3(0, -3, 2.697)));
	pl->buffer(prgm, plane, NULL, planeEL, planeCountOfVer, planeCountOfEl);
	
	pl->setTexture("../imgs/text.bmp", GL_RGB);

	w->addModel(sp2);
	w->addModel(sp3);
  //draw loop
	ge::util::Timer<float>timer;
  mainLoop->setIdleCallback([&]() {
	  auto const frameTime = timer.elapsedFromLast();
	  w->updateSurface(frameTime);
	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
	  
	  glDisable(GL_DEPTH_TEST);
	  glEnable(GL_STENCIL_TEST);
	  glStencilFunc(GL_ALWAYS, 1, 0);
	  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	  glEnable(GL_CULL_FACE);
	  glCullFace(GL_BACK);
	  w->Draw(false);
	  glDisable(GL_STENCIL_TEST); 


	  glEnable(GL_DEPTH_TEST);
	  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	  glEnable(GL_BLEND);
	  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	  glDisable(GL_CULL_FACE);
	  w->drawReflectRefract(true);

	  glEnable(GL_STENCIL_TEST);
	  glStencilFunc(GL_EQUAL, 1, 0xff);
	  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	  glEnable(GL_CULL_FACE);
	  glCullFace(GL_BACK);
	  prgm->use();
	  ls->glUniformLight(c->getPosition());
	  aq->draw(1.3333);
	  sp->draw(1.3333);
	  sp2->draw(1.3333);
	  sp3->draw(1.3333);
	  glStencilFunc(GL_EQUAL, 0, 0xff);
	  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	  prgm->use();
	  ls->glUniformLight(c->getPosition());
	  aq->draw(1);
	  sp->draw(1);
	  sp2->draw(1);
	  sp3->draw(1);	
	  glDisable(GL_STENCIL_TEST);
	 
	  w->Draw(false);
	  glDisable(GL_BLEND);
    window->swap();
  });

  (*mainLoop)();

  return EXIT_SUCCESS;
}
