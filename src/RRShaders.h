#pragma once
#include <string>
std::string static const RRVSSrc = R".(
#version 450

layout(location=0)in vec3 position;
layout(location=1)in vec3 normal;
layout(location=2)in vec3 TexCoord;

uniform mat4 MVP;
uniform mat4 modelMatrix;
uniform int Type;
uniform int ReflectOrRefract;

flat out int typ;
out vec3 tc;
out vec3 pos;
out vec3 norm;
float gl_ClipDistance[1];
void main() {
	gl_Position = MVP*vec4(position,1);
	if(ReflectOrRefract==1){
		gl_ClipDistance[0] = dot(modelMatrix * vec4(position,1), vec4(0,1,0,0));
	}
	if(Type ==0){
		tc = TexCoord;
	}
	typ = Type;
	pos = position;
	norm = normal;	
	
}).";

std::string static const RRFSSrc = R".(
#version 450
in vec3 tc;
in vec3 pos;
in vec3 norm;
flat in int typ;

uniform sampler2D text;

layout(location =0)out vec4 fColor;
///////////////LIGHT///////////
uniform	vec3 LightColor;
uniform	vec3 Dir;
uniform	float AmbInt;
uniform	float DiffInt;
uniform vec3 CameraPos;

vec4 Light(){
	float factor = dot(normalize(norm),-Dir); 
	vec4 diffuse = vec4(0,0,0,0);
	vec4 spec = vec4(0,0,0,0);
	if(factor>0){
		vec3 CameraVertex = normalize(CameraPos - pos);
		vec3 reflectLight = normalize(reflect(Dir,norm));
		float specFactor = dot(CameraVertex,reflectLight);
		if(specFactor>0){
			specFactor = pow(specFactor,100);
			spec=vec4(LightColor*specFactor,1.f);
		}
		diffuse=vec4(LightColor * DiffInt * factor,1.f);
	}
	vec4 Amb = vec4(LightColor*AmbInt,1.0f);

	return  (Amb+diffuse+spec);

}
////////////MAIN/////////////
void main(){
	vec4 color = vec4(0,0,0,0);
	vec4  l =vec4(1,1,1,1);
	if(typ%10 == 0){
		color = texture2D(text,vec2(tc.x,tc.y));
	}else if(typ%10 == 1){
		color=vec4(1,0,0,1);
	}	
	fColor = color*Light();
}).";

