#pragma once
#include "Model.h"
#include <SDL2/SDL.h>
using namespace ge::gl;
void Model::setPosition(glm::mat4 m)
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			this->modelMatrix[i][j] = m[i][j];
		}
	}
}

glm::mat4 Model::getPosition()
{
	return this->modelMatrix;
}

void Model::setRRUniform(std::shared_ptr<ge::gl::Program> prgm)
{
	this->uniformRRMVP = prgm->getUniformLocation("MVP");
	this->uniformRRModelMatrix = prgm->getUniformLocation("modelMatrix");
	this->uniformRRType = prgm->getUniformLocation("Type");
}

void Model::drawRR(bool ReflectOrRefract)
{
	if (ReflectOrRefract) {
		glm::vec3 t(0, 2 * this->modelMatrix[3].y, 0);
		glm::mat4 Projection = c->getProjection();
		glm::mat4 View = c->getView();
		glm::mat4 MVP = Projection * View * glm::translate(glm::scale(this->modelMatrix, glm::vec3(1, -1,1)),t);
		glUniformMatrix4fv(this->uniformRRMVP, 1, GL_FALSE, &MVP[0][0]);
	}
	else {
		glm::mat4 Projection = c->getProjection();
		glm::mat4 View = c->getView();
		glm::mat4 MVP = Projection * View * glm::scale(this->modelMatrix,glm::vec3(1.3333,1.3333,1.3333));
		glUniformMatrix4fv(this->uniformMVP, 1, GL_FALSE, &MVP[0][0]);
	}
	glUniformMatrix4fv(this->uniformRRModelMatrix, 1, GL_FALSE, &this->modelMatrix[0][0]);
	ge::gl::glBindVertexArray(this->vaoAq);
	glUniform1i(this->uniformRRType, this->type);
	if (type == 0) {
		if (texture != NULL) {
			texture->bind(0);
		}
		glBindBuffer(GL_ARRAY_BUFFER, vertexDataAq);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (const void*)(sizeof(float) * 3));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (const void*)(sizeof(float) * 6));
	}
	else if (type == 1) {
		glBindBuffer(GL_ARRAY_BUFFER, vertexDataAq);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SphereVertex), 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SphereVertex), (const void*)(sizeof(float) * 3));

	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexAq);
	ge::gl::glDrawElements(GL_TRIANGLES, this->CountOfEl, GL_UNSIGNED_SHORT, 0);
	ge::gl::glBindVertexArray(0);
}



void Model::setControler(controler * con)
{
	c = con;
}

void Model::buffer(std::shared_ptr<ge::gl::Program> prgm, const Point* vertex, const SphereVertex * sphereVeretex,const unsigned short element[][3], int countOfVertex, int countOfElemetn )
{
	//this->MVPaq = glm::translate(glm::mat4(1.0f),glm::vec3(0,0,0));

	this->CountOfEl = countOfElemetn;
	if (vertex!= NULL) {
		this->type = 0;
		glGenBuffers(1, &vertexDataAq);
		glBindBuffer(GL_ARRAY_BUFFER, vertexDataAq);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * countOfVertex, &vertex[0], GL_STATIC_DRAW);
	}
	else {
		this->type = 1;
		glGenBuffers(1, &vertexDataAq);
		glBindBuffer(GL_ARRAY_BUFFER, vertexDataAq);
		glBufferData(GL_ARRAY_BUFFER, sizeof(SphereVertex) * countOfVertex, &sphereVeretex[0], GL_STATIC_DRAW);
	}
	glGenBuffers(1, &this->indexAq);
	glBindBuffer(GL_ARRAY_BUFFER, this->indexAq);
	glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned short) * this->CountOfEl, &element[0], GL_STATIC_DRAW);
	
	glGenVertexArrays(1, &vaoAq);
	ge::gl::glBindVertexArray(vaoAq);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	if (type == 0) {
		glEnableVertexAttribArray(2);
	}
	this->prgm = prgm;
	this->uniformMVP = this->prgm->getUniformLocation("MVP");
	this->uniformType = this->prgm->getUniformLocation("Type");
	this->uniformModelMatrix = this->prgm->getUniformLocation("modelMatrix");

}


void Model::draw(float scale)
{

	glm::mat4 Projection = c->getProjection();
	glm::mat4 View = c->getView();
	glm::mat4 MVP = Projection * View * glm::scale(this->modelMatrix,glm::vec3(scale));
	glUniformMatrix4fv(this->uniformMVP, 1,GL_FALSE,&MVP[0][0]);
	glUniformMatrix4fv(this->uniformModelMatrix, 1, GL_FALSE, &this->modelMatrix[0][0]);
	ge::gl::glBindVertexArray(this->vaoAq);
	glUniform1i(this->uniformType, this->type);
	if (type == 0) {
		if (texture != NULL) {
			texture->bind(0);
		}
		glBindBuffer(GL_ARRAY_BUFFER, vertexDataAq);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (const void*)(sizeof(float) * 3));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (const void*)(sizeof(float) * 6));
	}
	else if (type == 1) {
		glBindBuffer(GL_ARRAY_BUFFER, vertexDataAq);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SphereVertex), 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SphereVertex), (const void*)(sizeof(float) * 3));

	}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,indexAq);
		ge::gl::glDrawElements(GL_TRIANGLES,this->CountOfEl,GL_UNSIGNED_SHORT, 0);
		ge::gl::glBindVertexArray(0);

}

void Model::setTexture(std::string file,GLenum format)
{

	SDL_Surface *surface = SDL_LoadBMP(file.c_str());
	if (surface == NULL) {
		std::cout << "UNLOADED TEXTURE" << std::endl;
	}
	texture =new ge::gl::Texture(GL_TEXTURE_2D, GL_RGBA8, 0, surface->w, surface->h);
	texture->texParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
	texture->texParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
	texture->texParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	texture->texParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	texture->setData2D(surface->pixels, GL_BGR, GL_UNSIGNED_BYTE, 0, GL_TEXTURE_2D, 0, 0, surface->w, surface->h, surface->w);

	texture->generateMipmap();

}

Model::Model()
{
}


Model::~Model()
{
}
