#pragma once

#include "StepManager.h"
#include "LogParser.h"
#include "FileDialog.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <string>

class Application
{
public:
	Application(int width, int height);
	~Application();

	void Run();
	void Update();

	void Save();
	void Load();

	void SetImGuiStyle();

private:
	GLFWwindow* window_;
	bool running_ = true;
	int window_x_ = 400;
	int window_y_ = 400;
	int window_width_;
	int window_height_;

	StepManager step_manager_;
	LogParser log_parser_;
	std::unique_ptr<FileDialog> file_dialog_;

	//step manager
	// -steps
	//open file dialog -> maybe only create on stack if necessary, i.e no save file stating it.
	//file parser
	//-log_file_path
	//-end_of_file
	//-all the methods to parse file
	//
};