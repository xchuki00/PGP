#include "LightSource.h"
void LightSource::glGetUniformLocationLight(std::shared_ptr<ge::gl::Program> prgm)
{

	this->uniformDir= prgm->getUniformLocation("Dir");
	this->uniformColor = prgm->getUniformLocation("LightColor");
	this->uniformAI = prgm->getUniformLocation("AmbInt");
	this->uniformDI = prgm->getUniformLocation("DiffInt");
	this->uniformCameraPos = prgm->getUniformLocation("CameraPos");

}

void LightSource::glUniformLight(glm::vec3 CameraPos)
{

	ge::gl::glUniform3fv(this->uniformDir,1, &this->dir[0]);
	ge::gl::glUniform3fv(this->uniformColor,1, &this->color[0]);
	ge::gl::glUniform1f(this->uniformAI, this->ai);
	ge::gl::glUniform1f(this->uniformDI, this->di);
	ge::gl::glUniform3fv(this->uniformCameraPos, 1, &CameraPos[0]);
}

void LightSource::glGetUniformLocationLightWater(std::shared_ptr<ge::gl::Program> prgm)
{
	this->uniformDirWater = prgm->getUniformLocation("Dir");
	this->uniformColorWater = prgm->getUniformLocation("LightColor");
	this->uniformAIWater = prgm->getUniformLocation("AmbInt");
	this->uniformDIWater = prgm->getUniformLocation("DiffInt");
	this->uniformCameraPosWater = prgm->getUniformLocation("CameraPos");

}

void LightSource::glUniformLightWater(glm::vec3 CameraPos)
{
	ge::gl::glUniform3fv(this->uniformDirWater, 1, &this->dir[0]);
	ge::gl::glUniform3fv(this->uniformColorWater, 1, &this->color[0]);
	ge::gl::glUniform1f(this->uniformAIWater, this->ai);
	ge::gl::glUniform1f(this->uniformDIWater, this->di);
	ge::gl::glUniform3fv(this->uniformCameraPosWater, 1, &CameraPos[0]);
}

void LightSource::glGetUniformLocationLightRR(std::shared_ptr<ge::gl::Program> prgm)
{
	this->uniformDirRR = prgm->getUniformLocation("Dir");
	this->uniformColorRR = prgm->getUniformLocation("LightColor");
	this->uniformAIRR = prgm->getUniformLocation("AmbInt");
	this->uniformDIRR = prgm->getUniformLocation("DiffInt");
	this->uniformCameraPosRR = prgm->getUniformLocation("CameraPos");
}

void LightSource::glUniformLightRR(glm::vec3 CameraPos)
{
	ge::gl::glUniform3fv(this->uniformDirRR, 1, &this->dir[0]);
	ge::gl::glUniform3fv(this->uniformColorRR, 1, &this->color[0]);
	ge::gl::glUniform1f(this->uniformAIRR, this->ai);
	ge::gl::glUniform1f(this->uniformDIRR, this->di);
	ge::gl::glUniform3fv(this->uniformCameraPosRR, 1, &CameraPos[0]);
}

LightSource::LightSource(glm::vec3 dir, glm::vec3 color, float AmbientIntesity,float DifuseIntesity)
{
	this->ai = AmbientIntesity;
	this->di = DifuseIntesity;
	this->dir = glm::normalize(dir);
	this->color = glm::normalize(color);
}

LightSource::~LightSource()
{
}
