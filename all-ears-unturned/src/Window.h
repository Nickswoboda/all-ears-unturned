#pragma once

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
class Window
{
public:
	Window(int width, int height);

	static bool IsFocused();

	void Move(int x, int y);
	void ResizeHeight(int height);
	void UpdateSize();

	static GLFWwindow* glfw_window_;
	int x_pos_ = 0;
	int y_pos_ = 0;
	int width_ = 0;
	int height_ = 0;
	bool collapsed_ = false;
	bool movable_ = true;
};