#version 460 core

//Output is fragColor 
out vec4 fragColor;

uniform vec3 RGB;
uniform float transparency;

void main(){
	fragColor = vec4(RGB, transparency);
}