#pragma once

/// init library or exit on failure, usefull in main function since glfwTerminate() is called atexit()
void glfw_init(GLFWerrorfun on_error);

/// main window create (context)
GLFWwindow* glfw_window_init(int w, int h, const char *title);
