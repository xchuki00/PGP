#pragma once
#include <GPUEngine/geGL/StaticCalls.h>
#include <GPUEngine/geGL/geGL.h>
#include <GPUEngine/geAd/SDLWindow/SDLWindow.h>
#include <GPUEngine/geAd/SDLWindow/SDLMainLoop.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtc/matrix_access.hpp>
class LightSource
{


	GLint uniformDir;
	GLint uniformColor;
	GLint uniformAI;
	GLint uniformDI;
	GLint uniformCameraPos;

	GLint uniformDirWater;
	GLint uniformColorWater;
	GLint uniformAIWater;
	GLint uniformDIWater;
	GLint uniformCameraPosWater;

	GLint uniformDirRR;
	GLint uniformColorRR;
	GLint uniformAIRR;
	GLint uniformDIRR;
	GLint uniformCameraPosRR;
public:
	glm::vec3 dir;
	glm::vec3 color;
	float ai = 1.0f;
	float di = 1.0f;
	void glGetUniformLocationLight(std::shared_ptr<ge::gl::Program> prgm);
	void glUniformLight(glm::vec3 CameraPos);
	void glGetUniformLocationLightWater(std::shared_ptr<ge::gl::Program> prgm);
	void glUniformLightWater(glm::vec3 CameraPos);
	void glGetUniformLocationLightRR(std::shared_ptr<ge::gl::Program> prgm);
	void glUniformLightRR(glm::vec3 CameraPos);
	LightSource(glm::vec3 dir, glm::vec3 color, float AmbientIntesity, float DifuseIntesity);
	~LightSource();
};

