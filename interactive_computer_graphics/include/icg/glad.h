#pragma once

#include <stdio.h>
#include <string.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

struct shader_prog {
	GLuint prog;
	GLuint vs;
	GLuint fs;
};

// \return a glad version or 0 on fail
static inline int glad_init()
{
	return gladLoadGL(glfwGetProcAddress);
}

static inline int shader_create(int typ, const char *shader_code, GLuint *shader)
{
	int r = 0;
	GLint is_compiled;
	GLint log_len;
	GLchar* log;
	GLuint s;

	s = glCreateShader(typ);
	if (s == 0) {
		r = glGetError();
		fprintf(stderr, "glCreateShader() fail: %d\n", r);
		return r;
	}

	glShaderSource(s, 1, &shader_code, NULL);
	r = glGetError();
	if (r) {
		fprintf(stderr, "glShaderSource() fail: %d\n", r);
		goto fail;
	}

	glCompileShader(s);

	// superflous???
	r = glGetError();
	if (r) {
		fprintf(stderr, "glCompileShader() fail: %d\n", r);
		goto fail;
	}

	glGetShaderiv(s, GL_COMPILE_STATUS, &is_compiled);
	if (is_compiled == GL_FALSE) {
		r = GL_INVALID_VALUE;

		glGetShaderiv(s, GL_INFO_LOG_LENGTH, &log_len);
		if (log_len > 0) {
			log = (GLchar*)malloc(log_len + 1);
			if (log) {
				glGetShaderInfoLog(s, log_len, NULL, log);
				log[log_len] = 0;
				// message has newline (always???)
				fprintf(stderr, "glCompileShader() fail: %s", log);
				free(log);
				goto fail;
			} else {
				fprintf(stderr, "malloc(%u) fail\n", log_len + 1);
			}
		}

		fprintf(stderr, "glCompileShader() fail: unknown\n");
		goto fail;
	}

	*shader = s;
	return 0;

fail:
	glDeleteShader(s);
	return r;
}

static inline void shader_prog_clean(struct shader_prog *prog)
{
	if (prog->prog)
		glDeleteProgram(prog->prog);

	if (prog->vs)
		glDeleteShader(prog->vs);

	if (prog->fs)
		glDeleteShader(prog->fs);

	memset(prog, 0, sizeof(*prog));
}

static inline int shader_prog_create(const char *vertex_code, const char *fragment_code, struct shader_prog *prog)
{
	int r;
	GLint val;

	memset(prog, 0, sizeof(*prog));

	prog->prog = glCreateProgram();
	if (!prog->prog) {
		r = glGetError();
		fprintf(stderr, "glCreateProgram() fail: %d\n", r);
		goto fail;
	}

	if (vertex_code) {
		r = shader_create(GL_VERTEX_SHADER, vertex_code, &prog->vs);
		if (r)
			goto fail;
	}

	if (fragment_code) {
		r = shader_create(GL_FRAGMENT_SHADER, fragment_code, &prog->fs);
		if (r)
			goto fail;
	}

	glAttachShader(prog->prog, prog->vs);
	if ((r = glGetError())) {
		fprintf(stderr, "glAttachShader(vs) fail: %d", r);
		goto fail;
	}

	glAttachShader(prog->prog, prog->fs);
	if ((r = glGetError())) {
		fprintf(stderr, "glAttachShader(fs) fail: %d", r);
		goto fail;
	}

	glLinkProgram(prog->prog);
	glGetProgramiv(prog->prog, GL_LINK_STATUS, &val);
	if (val == GL_FALSE) {
		fprintf(stderr, "glLinkProgram() fail: %d", r);
		goto fail;
	}

	return 0;

fail:
	shader_prog_clean(prog);
	return r;
}

static inline int shader_prog_bind(struct shader_prog *prog)
{
	glUseProgram(prog->prog);
	return 0;
}

