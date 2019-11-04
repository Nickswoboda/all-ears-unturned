#pragma once

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

class Application
{
public:
	Application();
	~Application();

	void Run();

private:
	GLFWwindow* window_;
};