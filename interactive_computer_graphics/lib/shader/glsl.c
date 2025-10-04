// build-in shaders

#include <icg/glsl.h>

const char *GLSL_SHADER_PRJ_02_VERT = "#version 460 core\n \
\n \
layout(location=0) in vec3 pos;\n \
\n \
uniform mat4 mvp;\n \
\n \
void main()\n \
{\n \
	// w/o mvp\n \
	// gl_Position = vec4(pos * 0.05, 1);\n \
	\n \
	// using mvp\n \
	gl_Position = mvp * vec4(pos, 1);\n \
}";

const char *GLSL_SHADER_SIMPLE_FRAG = "#version 460 core\n \
\n \
layout(location = 0) out vec4 color;\n \
\n \
void main()\n \
{\n \
	color = vec4(1, 0, 0, 1);\n \
}";

const char *GLSL_SHADER_SIMPLE_VERT = "#version 460 core\n \
\n \
layout(location=0) in vec3 pos;\n \
\n \
uniform mat4 mvp;\n \
\n \
void main()\n \
{\n \
	gl_Position = mvp * vec4(pos, 1);\n \
}";

