#version 330 core

uniform sampler2D depthTex;
uniform sampler2D normTex;

uniform vec2 pixelSize;
uniform vec3 cameraPos;

uniform float lightRadius;
uniform vec3 lightPos;
uniform vec4 lightColour;
uniform mat4 inverseProjView;

out vec4 diffuseOutput;
out vec4 specularOutput;

void main(void){
	
}