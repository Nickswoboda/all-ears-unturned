#include "Window.h"
#include <iostream>

Window::Window(int width, int height)
	:width_(width), height_(height)
{
	if (!glfwInit()) {
		std::cout << "Could not initialize GLFW";
	}

	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

	glfw_window_ = glfwCreateWindow(width_, height_, "All Ears Unturned", NULL, NULL);
	if (!glfw_window_) {
		glfwTerminate();
		std::cout << "Could not create window";
	}

	glfwSetWindowPos(glfw_window_, x_pos_, y_pos_);

	glfwMakeContextCurrent(glfw_window_);
}

void Window::Update()
{
	glfwSetWindowPos(glfw_window_, x_pos_, y_pos_);
	glfwSetWindowSize(glfw_window_, width_, height_);
}

