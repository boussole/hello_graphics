/*
In this project we will display the vertices of an object. This will involve generating a vertex buffer, computing the camera transformations, and very simple GLSL shaders.

Requirements

The requirements of this project are separated into three steps. You might be able to verify your implementation at the end of each step, but there is no guarantee that you will have a working implementation before you complete all three steps.

    Step 1: Vertex buffers
        Open and read the vertex data from an .obj file.
        You can use this teapot.obj file for your tests.
        You can use the cyTriMesh code release for parsing the obj file.
        Your program must take the name of the .obj file as its first command-line argument.
        Generate and bind a vertex array object.
        You can use the GLEW library for initializing OpenGL extension functions.
        Do not forget to call glewInit after you create the OpenGL window.
        Generate a vertex buffer and set its data using the vertices you read from the .obj file.
        You can refer to this tutorial regarding the steps for generating the vertex buffer.
        Note that this tutorial does not include a vertex array object,but your implementation should.
        You can find a detailed description of vertex array objects and how to use them here.
        Draw the contents of the buffer in your display function.
    At the end of this step you should see a single (white) point at the center of the screen,
    but depending on the settings of your OpenGL environment, you may see a completely blank screen instead.

    Step 2: GLSL shaders
        Write a simple vertex shader and a simple fragment shader.
        The vertex shader should transform all vertices by multiplying them with a model-view-projection matrix.
        The fragment shader should simply return a constant color value.
        You can use any color value you like.
        Compile the shaders into a GLSL program and use it for your draw call.
        You can use the cyGL code release for handling the shaders.
    If your vertex shader simply uses the given vertex position (without multiplying with any matrix),
    you should see a single point at the center of the screen at the end of this step.
    If your vertex shader multiplies the given vertex positions by 0.05,
    you should see all vertices of the teapot, as shown below.

    Step 3: Transformations
        Set the camera transformation using two rotation angles and one translation that determines the distance of the object from the camera.
        You can use the cyMatrix code release for setting up your transformation matrices.
        The left mouse button (and drag) should adjust the camera angles.
        The right mouse button (and drag) should adjust the camera distance.
        Use perspective transformation for projection.
        Send the matrix to your vertex shader as a uniform parameter.

Additional Requirements for CS 6610 Students
    Pressing the F6 key should recompile your GLSL shaders.
    Center the object (using its bounding box) by adjusting the transformation matrix.

Optional
    Pressing the P key switches between perspective and orthogonal transformation.
    When using orthogonal transformation, use one over camera distance as a uniform scale factor.

Notes
You can use GLFW instead of FreeGLUT, if you are having problems with GLUT/FreeGLUT or if you would prefer GLFW.
Mac OS X users can follow this tutorial for installing GLEW.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <icg/glad.h>
#include <icg/glsl.h>
#include <icg/glfw.h>
#include <icg/common.h>

#include <linmath.h>
#include <wavefront_obj.h>

GLFWwindow* window;
int width, height;

GLuint vbo = 0;
GLuint vao = 0;
struct wf_obj obj;
struct shader_prog prog;
GLint pos_location;
GLint mvp_location;
int nr_vertices;

int left_pressed, right_pressed;
float delta_time = 0.0f;
float last_frame_at = 0.0f;

float fov = 45; // a usual angle
double yaw = -90.0f;
double pitch = 0.0f;
double last_x, last_y;

vec3 eye = {0, 0, 30.0f}, center = {0, 0, -1}, up = {0.0f, 1.0f, 0.0f};

void glfw_on_framebuffer_resize(GLFWwindow*, int w, int h)
{
	printf("on_resize: w=%d h=%d\n", w, h);
	glViewport(0, 0, w, h);
	width = w;
	height = h;
	last_x = width / 2;
	last_y = height / 2;
}

void glfw_on_key_action(GLFWwindow*, int key, int scancode, int action, int mods)
{
	vec3 s;
	float speed = 10.5f * delta_time;

	printf("key=%d scancode=%d action=%d mods=%d\n", key, scancode, action, mods);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		vec3_scale(s, center, speed);
		vec3_add(eye, eye, s);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		vec3_scale(s, center, speed);
		vec3_sub(eye, eye, s);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		vec3_mul_cross(s, center, up);
		vec3_norm(s, s);
		vec3_scale(s, s, speed);
		vec3_sub(eye, eye, s);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		vec3_mul_cross(s, center, up);
		vec3_norm(s, s);
		vec3_scale(s, s, speed);
		vec3_add(eye, eye, s);
	}
}

void glfw_on_mouse_button(GLFWwindow*, int button, int action, int mods)
{
	printf("button=%d action=%d mods=%d\n", button, action, mods);

	if (button == 0) {
		left_pressed = action == 1;
	} else {
		right_pressed = action == 1;
	}
}

// fixme: some spontaneous jumps at the start of action, offset calculation smothing not implemented
void on_yaw_pitch_change(double xpos, double ypos)
{
	vec3 direction;
	const float sensitivity = 0.1f;

	float xoffset = xpos - last_x;
	float yoffset = last_y - ypos; // reversed since y-coordinates range from bottom to top
	last_x = xpos;
	last_y = ypos;

	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if(pitch > 89.0f)
		pitch = 89.0f;
	if(pitch < -89.0f)
		pitch = -89.0f;

	direction[0] = cos(degrees_to_radians(yaw)) * cos(degrees_to_radians(pitch));
	direction[1] = sin(degrees_to_radians(pitch));
	direction[2] = sin(degrees_to_radians(yaw)) * cos(degrees_to_radians(pitch));

	vec3_norm(center, direction);
}

double last_zoom_y;

void on_zoom(double ypos)
{
	double p = last_zoom_y - ypos;
	last_zoom_y = ypos;

	fov -= (float)p;

	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}

void glfw_on_cursor_position(GLFWwindow* w, double xpos, double ypos)
{
	if (left_pressed) {
		glfwGetCursorPos(w, &xpos, &ypos);
		printf("xpos=%f ypos=%f\n", xpos, ypos);
		on_yaw_pitch_change(xpos, ypos);
	}

	if (right_pressed) {
		glfwGetCursorPos(w, &xpos, &ypos);
		printf("xpos=%f ypos=%f\n", xpos, ypos);
		on_zoom(ypos);
	}
}

void prepare()
{
	int r;

	r = shader_prog_create(GLSL_SHADER_PRJ_02_VERT, GLSL_SHADER_SIMPLE_FRAG, &prog);
	if (r)
		exit(EXIT_FAILURE);

	shader_prog_bind(&prog);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	nr_vertices = obj.nr_vertices;
	glNamedBufferData(vbo, sizeof(struct wf_vertex) * obj.nr_vertices, obj.vertices, GL_STATIC_DRAW);

	printf("size=%zu\n", sizeof(struct wf_vertex) * obj.nr_vertices);

	pos_location = glGetAttribLocation(prog.prog, "pos");
	printf("'pos' location=%d\n", pos_location);

	mvp_location = glGetUniformLocation(prog.prog, "mvp");
	printf("'mvp' location=%d\n", mvp_location);

	glEnableVertexArrayAttrib(vao, pos_location);
	glVertexAttribPointer(pos_location, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glfwGetFramebufferSize(window, &width, &height);

	last_x = width / 2;
	last_y = height / 2;
	last_zoom_y = height / 2;

	printf("widdows w=%d h=%d\n", width, height);
}

void render()
{
	mat4x4 v, p, mvp;
	float current_frame_at = glfwGetTime();
	delta_time = current_frame_at - last_frame_at;
	last_frame_at = current_frame_at;
	const float ratio = width / (float) height;

	glfwGetFramebufferSize(window, &width, &height);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	// using Model as-is, no changes no multiplications
	mat4x4_identity(v);

	// ex 1: static, move back (-z) by view matrix
	//mat4x4_translate(v, 0, 0, -30.0f);

	// ex 2: around the center of world with radius 30
	// const float radius = 30.0f;
	// eye[0] = sin(glfwGetTime()) * radius;
	// eye[2] = cos(glfwGetTime()) * radius;
	// printf("x=%f z=%f\n", eye[0], eye[2]);
	// using camera view
	// mat4x4_look_at(v, eye, center, up);

	// ex 3: movement wasd and mouse
	vec3 d = {0, 0, 0};
	vec3_add(d, eye, center);

	printf("eye (%f %f %f) dir (%f %f %f) up (%f %f %f)\n", eye[0], eye[1], eye[2], d[0], d[1], d[2], up[0], up[1], up[2]);
	mat4x4_look_at(v, eye, d, up);

	mat4x4_identity(p);
	mat4x4_perspective(p, degrees_to_radians(fov), ratio, 1.f, 100.0f);
	//mat4x4_ortho(p, -ratio, ratio, -ratio, ratio, -1.f, 100.f);

	mat4x4_mul(mvp, p, v);

	glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) &mvp);
	glDrawArrays(GL_POINTS, 0, nr_vertices);
}

void clean()
{
	shader_prog_clean(&prog);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

int main(int argc, char *argv[])
{
	int r;

	if (argc < 2) {
		fprintf(stderr, "enter *.obj filename\n");
		exit(EXIT_FAILURE);
	}

	wf_obj_init(&obj);
	r = wf_obj_load(argv[1], &obj);
	if (r) {
		fprintf(stderr, "could not load vertices from '%s': %s (%d)\n", argv[1], strerror(r), r);
		exit(EXIT_FAILURE);
	}

	printf("loaded %d vertices\n", obj.nr_vertices);
	// wf_obj_dump(&obj);

	glfw_init(NULL);

	window = glfw_window_init(640, 480, "Transformation");
	if (!window) {
		exit(EXIT_FAILURE);
	}

	glfwSetFramebufferSizeCallback(window, glfw_on_framebuffer_resize);
	glfwSetKeyCallback(window, glfw_on_key_action);
	glfwSetMouseButtonCallback(window, glfw_on_mouse_button);
	glfwSetCursorPosCallback(window, glfw_on_cursor_position);

	if (!glad_init()) {
		fprintf(stderr, "glad_init() fail()\n");
		exit(EXIT_FAILURE);
	}

	prepare();

	while (!glfwWindowShouldClose(window)) {
		render();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	clean();
	glfwDestroyWindow(window);
	wf_obj_clean(&obj);

	return 0;
}
