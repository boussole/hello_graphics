#include <GLFW/glfw3.h>
#include <icg/glfw.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void glfw_on_error(int error, const char* description)
{
	fprintf(stderr, "glfw error: %s (%d)\n", description, error);
}

void glfw_init(GLFWerrorfun on_error)
{
	int r;

	if (!glfwInit()) {
		fprintf(stderr, "glfwInit() fail\n");
		exit(EXIT_FAILURE);
	}

	r = atexit(glfwTerminate);
	if (r) {
		fprintf(stderr, "atexit(glfwTerminate) fail: %s (%d)\n", strerror(r), r);
	}

	if (on_error)
		glfwSetErrorCallback(on_error);
	else
		glfwSetErrorCallback(glfw_on_error);
}

GLFWwindow* glfw_window_init(int w, int h, const char *title)
{
	GLFWwindow* window = glfwCreateWindow(w, h, title, NULL, NULL);
	if (!window) {
		fprintf(stderr, "glfwCreateWindow() fail");
		return NULL;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	return window;
}
