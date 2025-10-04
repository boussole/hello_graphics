#version 460 core

layout(location=0) in vec3 pos;

uniform mat4 mvp;

void main()
{
	// w/o mvp
	// gl_Position = vec4(pos * 0.05, 1);

	// using mvp
	gl_Position = mvp * vec4(pos, 1);
}