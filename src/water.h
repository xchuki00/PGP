#pragma once
#include <GPUEngine/geGL/StaticCalls.h>
#include <GPUEngine/geGL/geGL.h>
#include <GPUEngine/geAd/SDLWindow/SDLWindow.h>
#include <GPUEngine/geAd/SDLWindow/SDLMainLoop.h>
#include<GPUEngine/geGL/GLSLNoise.h>
#include<GPUEngine/geUtil/Timer.h>
#include<GPUEngine/geCore/Dtemplates.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtc/matrix_access.hpp>
#include<glm/gtx/norm.hpp>
#include <limits>
#include "LightSource.h"
#include "controler.h"
#include "Model.h"
typedef struct WaterPoint {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec3 textCoord;
	float energy;
	int  index;
	WaterPoint(glm::vec3 position, glm::vec3 norm,glm::vec3 tc) {
		pos = position;
		normal = norm;
		textCoord = tc;
		energy = 0;
	}
	WaterPoint() {
		pos = glm::vec3(0.0f);
		normal = glm::vec3(0.0f);
		textCoord = glm::vec3(0.0f);
		energy = 0;
	}
};
class Water
{
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	std::shared_ptr<ge::gl::Program> prgm;
	std::shared_ptr<ge::gl::Program> compute;
	std::shared_ptr<ge::gl::Program> init;
	std::shared_ptr<ge::gl::Program> ReflectRefract;
	LightSource* ls = NULL;
	GLuint vao;
	GLuint index;
	ge::gl::Texture* reflectText = NULL;
	ge::gl::Texture* refractText = NULL;
	GLuint computeVertexData;
	GLuint computeOut;
	////fragment a vertex uniform
	GLint uniformMVP;
	GLint uniformWaterColor;
	GLint uniformWindowWidth;
	GLint uniformWindowHeight;
	GLint uniformRefractText;
	GLint uniformReflectText;

	GLint uniformRefectOrRefract;
	////compute shader uniform
	GLint uniformWidht;
	GLint uniformHeight;
	GLint uniformPosOfPunch;
	GLint uniformEnergyOfPunch;
	GLint uniformPunch;
	GLint uniformDeltaTime;
	GLint uniformSize;
	////init shader uniform
	GLint uniformWidhtInit;
	GLint uniformHeightInit;

	std::vector<Model*> models;
	int textureSizeCoeficient = 10;
	controler *c=NULL;
	int push = -1;
	int width;
	int height;
	float size=1;

	void InitCompute();
public:
	std::vector<WaterPoint> points;
	std::vector<unsigned short> element;

	int getCountofSurfacePoints();
	void drawReflectRefract(bool ReflectOrRefract);
	void addModel(Model *m);
	void updateSurface(float time);
	void pushToPoint(int indexOfPoint);
	void buffer();
	void compileShaders();
	void setLightSource(LightSource*lightsource);
	void setControler(controler *controler);
	void setTexture();
	void resizeTexture();
	void Draw(bool toStencil);
	void Parser(int widhtPoints, int heightPoints, float height);
	Water(glm::vec3 Cornerpoints[],int countOfPoint,glm::vec3 normal,int widhtPoints, int heightPoints,float height);
	~Water();
};
struct pos {
	float x, y, z;
};