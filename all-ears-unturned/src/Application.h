#pragma once

#include "StepManager.h"
#include "LogParser.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <string>

class Application
{
public:
	Application();
	~Application();

	void Run();
	void Update();

	void Save();
	void Load();

private:
	GLFWwindow* window_;
	std::string log_file_path_;

	StepManager step_manager_;
	LogParser log_parser_;

	//step manager
	// -steps
	//open file dialog -> maybe only create on stack if necessary, i.e no save file stating it.
	//file parser
	//-log_file_path
	//-end_of_file
	//-all the methods to parse file
	//
};