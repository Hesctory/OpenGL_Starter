// Time to codify in glsl Language
// Syntax is similar to C++

// Declare version & profile
#version 460 core

// Declare inputs, you must put locations
layout(location = 0) in vec3 aPos;

// Matrices 
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
// Uniform reserved word means the programmer can pass 
// its value from the CPU program

void main(){ // for vertex shader, you don't need to declare output
			 // The output by default is the glPosition variable
	gl_Position = Projection * View * Model * vec4(aPos, 1.0);
}

