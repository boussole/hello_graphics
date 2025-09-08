// resources
// https://www.glfw.org/docs/latest/quick.html
// https://www.youtube.com/playlist?list=PLplnkTzzqsZS3R5DjmCQsqupu43oS9CFN (lessons 4-5)
// https://antongerdelan.net/opengl

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// already included
// #include <GLFW/glfw3.h>
#include <icg/glad.h>
#include <icg/glsl.h>
#include <icg/glfw.h>
#include <icg/common.h>
#include <linmath.h>

// vars

int dynamic_background_color;
float r_bg, g_bg, b_bg;

GLFWwindow* window;

GLuint vbo = 0;
GLuint vao = 0;

#if 0
GLuint program;
GLuint vs;
GLuint fs;
#endif
struct shader_prog prog;
GLint pos_location;
GLint mvp_location;
GLint nr_vertices;

mat4x4 m, p, mvp;

void glfw_on_framebuffer_resize(GLFWwindow*, int width, int height)
{
	printf("on_resize: w=%d h=%d\n", width, height);
	glViewport(0, 0, width, height);
}

void glfw_on_key_action(GLFWwindow*, int key, int scancode, int action, int mods)
{
	printf("key=%d scancode=%d action=%d mods=%d\n", key, scancode, action, mods);

	if (key == 257 && action == 1) {
		dynamic_background_color = !dynamic_background_color;
	}

	// exit by esc
	if (key == 256 && action == 1)
		glfwSetWindowShouldClose(window, 1);
}

void glfw_on_mouse_button(GLFWwindow*, int button, int action, int mods)
{
	printf("button=%d action=%d mods=%d\n", button, action, mods);
}

// no shader example, static coords of a triangle
void prepare_no_shader()
{
	static const float points[] = {
		0.0f,  0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f
	};

	nr_vertices = ARRAY_SIZE(points) / 3;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
}

void render_no_shader()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, nr_vertices);
}

void clean_no_shader()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

// project #1 of course - Hello World
// This first project is about setting up the environment for using OpenGL. We will create an OpenGL window and color it using basic OpenGL functions.
// Requirements
//    Use C/C++.
//    Use GLUT, FreeGLUT, GLFW, or another UI library of your choosing to create an OpenGL window.
//    The OpenGL viewport size must be explicitly specified.
//    Set the background color of the window using OpenGL calls.
//    The "Esc" key must close the window.
// Additional Requirements
//    Automatically animate the background color.
void prepare_proj_1()
{
	int r;

	r = shader_prog_create(GLSL_SHADER_SIMPLE_VERT, GLSL_SHADER_SIMPLE_FRAG, &prog);
	if (r)
		exit(EXIT_FAILURE);

	shader_prog_bind(&prog);

	float positions[] = {
		-0.8f, 0.4f, 0.0f,
		0.8f, 0.4f, 0.0f,
		0.8f, -0.4f, 0.0f,
		-0.8f, 0.4f, 0.0f,
		0.8f, -0.4f, 0.0f,
		-0.8f, -0.4f, 0.0f,
	};

	nr_vertices = ARRAY_SIZE(positions) / 3;
	printf("nr_vertices: %d\n", nr_vertices);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// glBufferData() can be used assuming vbo implicitly
	glNamedBufferData(vbo, sizeof(positions), positions, GL_STATIC_DRAW);

	pos_location = glGetAttribLocation(prog.prog, "pos");
	printf("'pos' location=%d\n", pos_location);

	// draw
	mvp_location = glGetUniformLocation(prog.prog, "mvp");
	printf("'mvp' location=%d\n", mvp_location);

	// glEnableVertexAttribArray() can be used assuming vba implicitly
	glEnableVertexArrayAttrib(vao, pos_location);
	glVertexAttribPointer(pos_location, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	// since vertex shader calculates a vertex position as mvp * pos, mvp must be non zero
	mat4x4_identity(mvp);
}

void render_proj_1()
{
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	const float ratio = width / (float) height;
	// printf("ratio %f\n", ratio);

	if (dynamic_background_color) {
		if (r_bg < 1.0f)
			r_bg += 0.01f;
		else if (g_bg < 1.0f)
			g_bg += 0.01f;
		else if (b_bg < 1.0f)
			b_bg += 0.01f;
		else
			r_bg = g_bg = b_bg = 0.0f;
	} else {
		r_bg = g_bg = b_bg = 0.0f;
	}

	glClearColor(r_bg, g_bg, b_bg, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	mat4x4 m, p, mvp;
	mat4x4_identity(m);
	mat4x4_rotate_Z(m, m, (float) glfwGetTime());
	mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
	mat4x4_mul(mvp, p, m);

	glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) &mvp);
	// glBindVertexArray(vab); ???
	glDrawArrays(GL_POINTS, 0, nr_vertices);
}

void clean_proj_1()
{
	shader_prog_clean(&prog);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

struct run {
	void (*prepare)();
	void (*render)();
	void (*clean)();
} run [] = {
	{prepare_no_shader, render_no_shader, clean_no_shader},
	{prepare_proj_1, render_proj_1, clean_proj_1},
};


int main(int argc, char *argv[])
{
	struct run *r = &run[0];

	if (argc > 1) {
		r = &run[atoi(argv[1])];
	}

	glfw_init(NULL);

	window = glfw_window_init(640, 480, "Hello ogl");
	if (!window) {
		exit(EXIT_FAILURE);
	}

	glfwSetFramebufferSizeCallback(window, glfw_on_framebuffer_resize);
	glfwSetKeyCallback(window, glfw_on_key_action);
	glfwSetMouseButtonCallback(window, glfw_on_mouse_button);

	if (!glad_init()) {
		fprintf(stderr, "glad_init() fail()\n");
		exit(EXIT_FAILURE);
	}

	r->prepare();

	while (!glfwWindowShouldClose(window)) {
		r->render();
		glfwSwapBuffers(window);
		glfwPollEvents();
		//printf("next %f\n", glfwGetTime());
	}

	r->clean();

	glfwDestroyWindow(window);

	return 0;
}
