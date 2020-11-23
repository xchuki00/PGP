#pragma once
#include <string>

std::string static const WaterVSSrc = R".(
#version 450

layout(location=0)in vec3 position;
layout(location=1)in vec3 normal;
layout(location=2)in vec3 TexCoord;
layout(location=3)in float energy;
layout(location=4)in int index;
uniform mat4 MVP;

out vec3 tc;
out vec4 pos;
out vec3 norm;
void main() {

	vec4 posT = vec4(position.x,position.y,position.z,1);
	gl_Position =MVP*posT;

	tc =TexCoord;
	pos = posT;
	norm = normal;	
}).";

std::string static const WaterFSSrc = R".(
#version 450
in vec3 tc;
in vec4 pos;
in vec3 norm;
uniform sampler2D reflectT;
uniform sampler2D refractT;

uniform vec4 waterColor;
uniform int windowWidth;
uniform int windowHeight;

out vec4 fColor;
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
		vec3 CameraVertex = normalize(CameraPos - pos.xyz);
		vec3 reflectLight = normalize(reflect(Dir,norm));
		float specFactor = dot(CameraVertex,reflectLight);
		if(specFactor>0){
			specFactor = pow(specFactor,100);
			spec=vec4(LightColor*specFactor,3.0f);
		}
		diffuse=vec4(LightColor * DiffInt * factor,1.0f);
	}
	vec4 Amb = vec4(LightColor*AmbInt,1.0f);
	
	return  (Amb+diffuse+spec);

}
////////////MAIN/////////////
void main(){
	float spec = (1 - abs(dot(normalize(norm),normalize( pos.xyz - CameraPos))))*0.5+0.4;
	vec2 projcoord = vec2(gl_FragCoord.x/windowWidth, gl_FragCoord.y/windowHeight);
	if(tc.z<0){
		fColor = waterColor;
	}else{
		vec4 reflectC = texture(reflectT,vec2(projcoord));
		vec4 refractC = texture(refractT,vec2(projcoord)); 
		
		fColor =reflectC*waterColor*Light();
		fColor.w = waterColor.w;
	}
		fColor.w = spec;
}).";



std::string static const WaterCSSrc = R".(
layout(local_size_x = 1,local_size_y = 1) in;
uniform int width;
uniform int height;
uniform float deltaTime;
uniform float size;
uniform vec2 posOfPunch;
uniform vec3 energyOfPunch;
uniform int punch;
struct WaterPoint{
	float x,y,z;
	float nx,ny,nz;
	float tx,ty,tz;
	float ex;
	int index;
};

layout(std430, binding = 6)buffer WPS{WaterPoint wps[];};
layout(std430, binding = 7)buffer WPSOUT{WaterPoint wpsOut[];};
vec3 calcNormal(ivec2 coord){
	uint c = (width * gl_GlobalInvocationID.y+gl_GlobalInvocationID.x)*(1);
	vec3 point = vec3(wps[c].x,wps[c].y,wps[c].z);
	vec3 forwardV = vec3(wps[c+width].x,wps[c+width].y,wps[c+width].z)-point;
	vec3 backV = vec3(wps[c-width].x,wps[c-width].y,wps[c-width].z)-point;
	vec3 rightV = vec3(wps[c+1].x,wps[c+1].y,wps[c+1].z)-point;
	vec3 leftV = vec3(wps[c-1].x,wps[c-1].y,wps[c-1].z)-point;
	vec3 ret;
	bool left =c%width==0;
	bool right = c%width==width-1;
	bool forward = c> width*(height-1);
	bool back = c<width;
	if(!left&&!right&&!forward&&!back){	//stredni body
		ret = normalize(cross(rightV,forwardV)+cross(leftV,backV)+cross(forwardV,leftV)+cross(backV,rightV));
	}else if(left&&!forward&&!back){//leva stena
		ret = normalize(cross(rightV,forwardV)+cross(backV,rightV));
	}else if(!left&&!right&&forward){//horni stena
		ret = normalize(cross(leftV,backV)+cross(backV,rightV));
	}else if(right&&!forward&&!back){//prava stena
		ret = normalize(cross(leftV,backV)+cross(forwardV,leftV));
	}else if(!left&&!right&&back){//dolni stena
		ret = normalize(cross(rightV,forwardV)+cross(forwardV,leftV));
	}else if(right&&back){//prava delni roh
		ret = normalize(cross(leftV,forwardV));
	}else if(right&&forward){//pravej horni roh
		ret =normalize(cross(leftV,backV));
	}else if(left&&forward){//levej horni roh
		ret =normalize(cross(rightV,backV));
	}else{//levej dolni roh
		ret = normalize(cross(rightV,forwardV));
	}

	return ret;
}
void main(){
	
	uint c = (width * gl_GlobalInvocationID.y+gl_GlobalInvocationID.x);
	if(c<width*height){
	float left = (c%width==0)?-wps[c].y:wps[c-1].y;
	float right = (c%width==width-1)?-wps[c].y:wps[c+1].y;
	float forward =(c>= width*(height-1))?-wps[c].y:wps[c+width].y;
	float back =(c<width)?-wps[c].y:wps[c-width].y;

	float f = 0.5*(left+right+forward+back - 4 * wps[c].y)/size;

	wpsOut[c].ex = wps[c].ex+f*deltaTime;
	wpsOut[c].y = (wps[c].y+wpsOut[c].ex);

	wpsOut[c].y -= wpsOut[c].y*0.001;
	vec3 norm = calcNormal(ivec2(0,0));
 	wpsOut[c].nx = norm.x;
	wpsOut[c].ny = norm.y;
	wpsOut[c].nz = norm.z;
	if(punch==c){
		wpsOut[c].ex = energyOfPunch.y;
		wpsOut[c+1].ex = energyOfPunch.y;
		wpsOut[c-1].ex = energyOfPunch.y;

		wpsOut[c+width].ex = energyOfPunch.y;
		wpsOut[c+width+1].ex = energyOfPunch.y;
		wpsOut[c+width-1].ex = energyOfPunch.y;

		wpsOut[c-width].ex = energyOfPunch.y;
		wpsOut[c-width+1].ex = energyOfPunch.y;
		wpsOut[c-width-1].ex = energyOfPunch.y;
	}			

	}

}).";


std::string static const WaterInitCSSrc = R".(
layout(local_size_x = 1,local_size_y = 1) in;
uniform int width;
uniform int height;
struct WaterPoint{
	float x,y,z;
	float nx,ny,nz;
	float tx,ty,tz;
	float ex;
	int index;
};

layout(std430, binding = 6)buffer WPS{WaterPoint wps[];};

void main(){
uint c = (width * gl_GlobalInvocationID.y+gl_GlobalInvocationID.x)*(1);
	wps[c].y = 0;
	wps[c].ny = 1;
	wps[c].ex = 0;


}).";
std::string static const csSrc =R".(
#version 450

layout(vertices=3)out;
in vec3 tc[];
in vec4 pos[];
in vec3 norm[];
out vec3 tcc[];
out vec4 posc[];
out vec3 normc[];
void main(){
  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	if(gl_InvocationID ==0){
	  gl_TessLevelOuter[0] = 8;
	  gl_TessLevelOuter[1] = 8;
	  gl_TessLevelOuter[2] = 8;
	  gl_TessLevelInner[0] = 8;
	}
	tcc[gl_InvocationID] =tc[0];
	posc[gl_InvocationID] = pos[0];
	normc[gl_InvocationID] = norm[0];	
}
).";

std::string static const esSrc =
R".(
#version 450
in vec3 tcc[];
in vec4 posc[];
in vec3 normc[];
out vec3 tc;
out vec4 pos;
out vec3 norm;
layout(triangles,equal_spacing, cw)in;

void main(){
gl_Position=(gl_TessCoord.x*gl_in[0].gl_Position+gl_TessCoord.y*gl_in[1].gl_Position+gl_TessCoord.z*gl_in[2].gl_Position);
	tc =tcc[0];
	pos = posc[0];
	norm = normc[0];	
}
).";