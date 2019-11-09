#pragma once

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
class Window
{
public:
	Window(int width, int height);
	
	void Update();

	GLFWwindow* glfw_window_ = nullptr;
	int x_pos_ = 0;
	int y_pos_ = 0;
	int width_ = 0;
	int height_ = 0;
};