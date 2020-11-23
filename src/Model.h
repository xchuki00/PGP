#pragma once
#include <GPUEngine/geGL/StaticCalls.h>
#include <GPUEngine/geGL/geGL.h>
#include <GPUEngine/geAd/SDLWindow/SDLWindow.h>
#include <GPUEngine/geAd/SDLWindow/SDLMainLoop.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtc/matrix_access.hpp>

#include "controler.h"
#include "Type.h"
class Model
{
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	std::shared_ptr<ge::gl::Program> prgm;
	GLuint vaoAq;

	GLint uniformMVP;
	GLint uniformType;
	GLint uniformModelMatrix;

	GLint uniformRRMVP;
	GLint uniformRRModelMatrix;
	GLint uniformRRType;

	ge::gl::Texture* texture = NULL;
	//GLuint texture;
	GLuint vertexDataAq;
	GLuint indexAq;
	controler* c;
	int type=0;
	int CountOfEl = 0;
	std::string name;
	bool falling = false;
public:

	void setPosition(glm::mat4 m);
	glm::mat4 getPosition();
	void setRRUniform(std::shared_ptr<ge::gl::Program> prgm);
	void drawRR(bool ReflectOrRefract);
	void setControler(controler* con);
	void buffer(std::shared_ptr<ge::gl::Program> prgm, const Point* vertex, const SphereVertex * sphereVeretex, const unsigned short element[][3], int countOfVertex, int countOfElemetn);
	void draw(float scale);
	void setTexture(std::string file, GLenum format);
	Model();
	~Model();
};

