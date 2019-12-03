#include "Window.h"
#include <iostream>

GLFWwindow* Window::glfw_window_ = nullptr;

Window::Window(int width, int height)
	:width_(width), height_(height)
{
	if (!glfwInit()) {
		std::cout << "Could not initialize GLFW";
	}

	//glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
	glfw_window_ = glfwCreateWindow(width_, height_, "All Ears Unturned", NULL, NULL);
	if (!glfw_window_) {
		glfwTerminate();
		std::cout << "Could not create window";
	}

	glfwSetWindowUserPointer(glfw_window_, this);
	glfwSetKeyCallback(glfw_window_, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
	
		if (key == GLFW_KEY_E && action == GLFW_PRESS) {
			auto* window = (Window*)glfwGetWindowUserPointer(glfw_window_);
			if (!window->collapsed_) {
				window->collapsed_ = true;
			}
			else {
				window->collapsed_ = false;
			}
		}
	});

	glfwMakeContextCurrent(glfw_window_);
	glfwSetWindowPos(glfw_window_, x_pos_, y_pos_);
}

bool Window::IsFocused()
{
	if (glfwGetWindowAttrib(glfw_window_, GLFW_FOCUSED)) {
		return true;
	}
	return false;
}

void Window::Move(int x, int y)
{
	x_pos_ += x;
	y_pos_ += y;
	glfwSetWindowPos(glfw_window_, x_pos_, y_pos_);
}

void Window::ResizeHeight(int height)
{
	height_ = height;
	UpdateSize();
}

void Window::UpdateSize()
{
	glfwSetWindowSize(glfw_window_, width_, height_);
}


