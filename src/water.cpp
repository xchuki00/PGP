#include "water.h"
#include "WaterShaders.h"
#include "RRShaders.h"
#include <cmath>
using namespace ge::gl;
void Water::InitCompute()
{
	this->init->use();
	glUniform1i(this->uniformWidhtInit, this->width);
	glUniform1i(this->uniformHeightInit, this->height);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, this->computeVertexData);

	glDispatchCompute(this->width, this->height, 1);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT| GL_SHADER_STORAGE_BARRIER_BIT);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}
int Water::getCountofSurfacePoints()
{
	return this->width*this->height;
}
void Water::drawReflectRefract(bool ReflectOrRefract)
{
	
	this->ReflectRefract->use();
	glEnable(GL_CLIP_PLANE0);
	if (ReflectOrRefract) {
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	ls->glUniformLightRR(c->getPosition());
	glUniform1i(this->uniformRefectOrRefract, (ReflectOrRefract) ? 1 : 0);
	for (Model*m : this->models) {
		m->drawRR(ReflectOrRefract);
	}
	if (ReflectOrRefract) {
		this->prgm->use();
		glUniform1i(uniformReflectText, 0);
		this->reflectText->bind(0);
		glCopyTexSubImage2D(this->reflectText->getTarget(), 0, 0, 0, 0, 0, c->getWindowWidth(), c->getWindowHeight());
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

}
void Water::addModel(Model * m)
{
	this->models.push_back(m);
	m->setRRUniform(this->ReflectRefract);
}
void Water::updateSurface(float time)
{
	this->compute->use();
	glUniform1i(this->uniformPunch, push);
	if (push!=-1) {
		glUniform2f(this->uniformPosOfPunch,this->points[push].pos.x,this->points[push].pos.x);
		glUniform3f(this->uniformEnergyOfPunch, 0, -0.15, 0);
		push = -1;
	}
	glUniform1i(this->uniformWidht, this->width);
	glUniform1i(this->uniformHeight, this->height);
	glUniform1f(this->uniformDeltaTime, time);
	glUniform1f(this->uniformSize, size);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER,6, this->computeVertexData);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, this->computeOut);
	glDispatchCompute(this->width, this->height, 1);
	
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT| GL_SHADER_STORAGE_BARRIER_BIT);
	glCopyNamedBufferSubData(this->computeOut, this->computeVertexData, 0, 0, sizeof(WaterPoint)*this->points.size());
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}
void Water::pushToPoint(int indexOfPoint)
{
	push = indexOfPoint;
}
void Water::buffer()
{

	glGenBuffers(1, &this->computeVertexData);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->computeVertexData);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(WaterPoint) * this->points.size(), &this->points[0], GL_STATIC_DRAW);

	glGenBuffers(1, &this->computeOut);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->computeOut);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(WaterPoint) * this->points.size(), &this->points[0], GL_STATIC_DRAW);

	glGenBuffers(1, &this->index);
	glBindBuffer(GL_ARRAY_BUFFER, this->index);
	glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned short) * this->element.size(), &element[0], GL_STATIC_DRAW);
	

	glGenVertexArrays(1, &vao);
	ge::gl::glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	this->uniformMVP = this->prgm->getUniformLocation("MVP");
	this->uniformWaterColor = this->prgm->getUniformLocation("waterColor");
	this->uniformWindowWidth = this->prgm->getUniformLocation("windowWidth");
	this->uniformWindowHeight = this->prgm->getUniformLocation("windowHeight");
	this->uniformReflectText = this->prgm->getUniformLocation("reflectT");
	this->uniformRefractText = this->prgm->getUniformLocation("refractT");

	this->uniformRefectOrRefract = this->ReflectRefract->getUniformLocation("ReflectOrRefract");

	this->uniformWidht= this->compute->getUniformLocation("width");
	this->uniformHeight = this->compute->getUniformLocation("height");
	this->uniformDeltaTime = this->compute->getUniformLocation("deltaTime");
	this->uniformPosOfPunch = this->compute->getUniformLocation("posOfPunch");
	this->uniformEnergyOfPunch = this->compute->getUniformLocation("energyOfPunch");
	this->uniformPunch = this->compute->getUniformLocation("punch");
	this->uniformSize = this->compute->getUniformLocation("size");

	this->uniformWidhtInit = this->init->getUniformLocation("width");
	this->uniformHeightInit = this->init->getUniformLocation("height");

	this->InitCompute();
	this->setTexture();
}

void Water::compileShaders()
{

	auto vs = std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER, WaterVSSrc);
	auto cs = std::make_shared<ge::gl::Shader>(GL_TESS_CONTROL_SHADER, csSrc);
	auto es = std::make_shared<ge::gl::Shader>(GL_TESS_EVALUATION_SHADER, esSrc);
	auto fs = std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER, WaterFSSrc);
	this->prgm = std::make_shared<ge::gl::Program>(vs,cs,es,fs);
	ls->glGetUniformLocationLightWater(this->prgm);

	vs = std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER, RRVSSrc);
	fs = std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER, RRFSSrc);
	this->ReflectRefract = std::make_shared<ge::gl::Program>(vs, fs);
	ls->glGetUniformLocationLightRR(this->ReflectRefract);

	auto com = std::make_shared<ge::gl::Shader>(GL_COMPUTE_SHADER, "#version 450\n", ge::gl::getNoiseSource(), WaterCSSrc);
	this->compute = std::make_shared<ge::gl::Program>(com);

	auto i = std::make_shared<ge::gl::Shader>(GL_COMPUTE_SHADER, "#version 450\n", ge::gl::getNoiseSource(), WaterInitCSSrc);
	this->init = std::make_shared<ge::gl::Program>(i);
	ls->glGetUniformLocationLightRR(this->ReflectRefract);
}

void Water::setLightSource(LightSource * lightsource)
{
	this->ls = lightsource;

}

void Water::setControler(controler * controler)
{
	this->c = controler;
}

void Water::setTexture()
{



	reflectText = new ge::gl::Texture(GL_TEXTURE_2D, GL_RGBA8, 0, c->getWindowWidth(), c->getWindowHeight());
	reflectText->texParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
	reflectText->texParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
	reflectText->texParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	reflectText->texParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	reflectText->generateMipmap();


	refractText = new ge::gl::Texture(GL_TEXTURE_2D, GL_RGBA8, 0, c->getWindowWidth(),c->getWindowHeight());
	refractText->texParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
	refractText->texParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
	refractText->texParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	refractText->texParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	refractText->generateMipmap();
}

void Water::resizeTexture()
{
	free(reflectText);
	reflectText = new ge::gl::Texture(GL_TEXTURE_2D, GL_RGBA8, 0, c->getWindowWidth(), c->getWindowHeight());
	reflectText->texParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
	reflectText->texParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
	reflectText->texParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	reflectText->texParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	reflectText->generateMipmap();

	free(refractText);
	refractText = new ge::gl::Texture(GL_TEXTURE_2D, GL_RGBA8, 0, c->getWindowWidth(), c->getWindowHeight());
	refractText->texParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
	refractText->texParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
	refractText->texParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	refractText->texParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	refractText->generateMipmap();
}

void Water::Draw(bool toStencil)
{
	
	
	this->prgm->use();

	ls->glUniformLightWater(c->getPosition());
	glm::mat4 Projection = c->getProjection();
	glm::mat4 View = c->getView();
	glm::mat4 MVP = Projection * View * this->modelMatrix;
	
	glUniformMatrix4fv(this->uniformMVP, 1, GL_FALSE, &MVP[0][0]);
	
	glUniform1i(uniformReflectText, 0);
	reflectText->bind(0);

	glUniform1i(uniformRefractText, 1);
	refractText->bind(1);

	glUniform1i(this->uniformWindowWidth, c->getWindowWidth());
	glUniform1i(this->uniformWindowHeight, c->getWindowHeight());
	glUniform4fv(this->uniformWaterColor,1, &glm::vec4(33.0f / 255.0f, 100.0f / 255.0f, 122.0f / 255.0f, 0.9)[0]);
	ge::gl::glBindVertexArray(this->vao);
	glBindBuffer(GL_ARRAY_BUFFER, this->computeOut);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(WaterPoint), 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(WaterPoint), (const void*)(sizeof(glm::vec3)));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(WaterPoint), (const void*)(sizeof(glm::vec3) * 2));
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(WaterPoint), (const void*)(sizeof(glm::vec3) * 3));
		glVertexAttribPointer(4, 1, GL_INT, GL_FALSE, sizeof(WaterPoint), (const void*)(sizeof(glm::vec3) * 3+sizeof(float)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index);
	if (toStencil) {
		glPatchParameteri(GL_PATCH_VERTICES, 3);
		ge::gl::glDrawElements(GL_PATCHES, 2*3*(this->width-1)*(this->height-1), GL_UNSIGNED_SHORT, 0);
	}
	else {
		glPatchParameteri(GL_PATCH_VERTICES, 3);
		ge::gl::glDrawElements(GL_PATCHES, this->element.size(), GL_UNSIGNED_SHORT, 0);

	}
	ge::gl::glBindVertexArray(0);
}

void Water::Parser(int widhtPoints, int heightPoints, float height)
{
	WaterPoint p2 = this->points[2];
	this->points.pop_back();
	WaterPoint p1 = this->points[1];
	this->points.pop_back();
	this->points[0].textCoord = glm::vec3(0.0f,0.0f,1.0f);
	p1.textCoord = glm::vec3(0.0f, 1.0f, 1.0f);
	p2.textCoord = glm::vec3(1.0f, 0.0f, 1.0f);
	this->size = glm::length2(p2.pos - this->points[0].pos)*glm::length2(p1.pos - this->points[0].pos);
	this->size /= ((widhtPoints - 1) * (heightPoints - 1));
	
	glm::vec3 stepYTc = (p1.textCoord - this->points[0].textCoord) / (float)(heightPoints-1);
	glm::vec3 stepYpos = (p1.pos - this->points[0].pos) / (float)(heightPoints-1);

	glm::vec3 stepXTc = (p2.textCoord - this->points[0].textCoord) / (float)(widhtPoints-1);
	glm::vec3 stepXpos = (p2.pos - this->points[0].pos) / (float)(widhtPoints-1);
	WaterPoint base = this->points[0];
	WaterPoint first = this->points[0];
	for (int i = 0; i < heightPoints; i++) {
		for (int j = 0; j < widhtPoints-1; j++) {
			this->points.push_back(WaterPoint(base.pos+stepXpos,base.normal,base.textCoord+stepXTc));
			this->points[this->points.size() - 1].index = this->points.size() - 1;
			base = this->points[this->points.size() - 1];
		}
		if (i != heightPoints - 1) {
			this->points.push_back(WaterPoint(first.pos + stepYpos, first.normal, first.textCoord + stepYTc));
			this->points[this->points.size() - 1].index = this->points.size() - 1;
			base = this->points[this->points.size() - 1];
			first = base;
		}
	}
	int index = 0;
	for (int i = 0; i < ((this->width - 1)*(this->height - 1)); i++) {
		if (i % 2 == 0) {
			this->element.push_back(index);this->element.push_back(index + this->width);
			this->element.push_back(index + this->width + 1);
			
		

			this->element.push_back(index);this->element.push_back(index + this->width + 1);
			this->element.push_back(index + 1);
			
		}
		else {
			this->element.push_back(index);this->element.push_back(index + this->width);
			this->element.push_back(index + 1);
			

			this->element.push_back(index+1);this->element.push_back(index + this->width);
			this->element.push_back(index + 1 + this->width);
			
		}
		if (index%width == width-2) {
			index += 2;
			if (index == width*(this->height - 1)) {
				break;
			}
		}else {
			index++;
		}
		
	}
	WaterPoint last(this->points[0].pos+glm::vec3(0,-height,0),glm::vec3(-0.5,0,0.5),glm::vec3(-1.0f));
	index = 1;
	last.index = this->points.size();
	this->points[0].textCoord.z = -0.01;
	this->points.push_back(last);
	int firstBottom = last.index;
	int step = 1;
	while (index!=0) {
		WaterPoint next(this->points[index].pos + glm::vec3(0, -height, 0),glm::vec3(0.0f),glm::vec3(-10000.0f));
		next.index = this->points.size();
		this->element.push_back(last.index);
		this->element.push_back(index-step);
		this->element.push_back(index);

		this->element.push_back(last.index);
		this->element.push_back(index);
		this->element.push_back(next.index);
		this->points[index].textCoord.z = 0;
		if (index < this->width) {
			if (index == this->width - 1) {
				next.normal = glm::vec3(0.5, 0, 0.5);
				index += this->width;
				step = this->width;
			}
			else {
				index++;
				next.normal = glm::vec3(0, 0, 1);
			}
		}else if (index%this->width == this->width - 1) {
			if (index == (this->height*this->width) - 1) {
				next.normal = glm::vec3(0.5, 0, -0.5);
				index--;
				step = -1;
			}
			else {
				next.normal = glm::vec3(1, 0, 0);
				index += this->width;
			}
		}
		else if (index >= (this->height-1)*this->width) {
			if (index == (this->height - 1)*this->width) {
				next.normal = glm::vec3(-0.5, 0, -0.5);
				index -= this->width;
				step = (-this->width);
			}
			else {
				next.normal = glm::vec3(0, 0, -1);
				index--;
			}
		}
		else {
				next.normal = glm::vec3(-1, 0, 0);
				index -= this->width;
		
		}
		//next.normal = glm::vec3(0, 1, 0);
		this->points.push_back(next);
		last = next;

	}
	this->element.push_back(last.index);
	this->element.push_back(0+width);
	this->element.push_back(0);

	this->element.push_back(last.index);
	this->element.push_back(0);
	this->element.push_back(firstBottom);


	this->element.push_back(firstBottom);
	this->element.push_back(firstBottom + width + this->height - 2);
	this->element.push_back(firstBottom + width * 2 + this->height - 3);

	this->element.push_back(firstBottom);
	this->element.push_back(firstBottom + width - 1);
	this->element.push_back(firstBottom + width+ this->height-2);
	
}



Water::Water(glm::vec3 Cornerpoints[], int countOfPoint, glm::vec3 normal,int widhtPoints, int heightPoints, float height)
{
	
	for (int i = 0; i < countOfPoint; i++) {
		this->points.push_back(WaterPoint(Cornerpoints[i], glm::vec3(0.0f,1.0f,0.0f), glm::vec3(1.0f)));
		this->points[i].index = i;
	}
	std::cout << "OK"<<this->points.size();

	this->width = widhtPoints;
	this->height = heightPoints;
	
	this->Parser(widhtPoints, heightPoints, height);
}

Water::~Water()
{
}
