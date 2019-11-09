#pragma once

#include "Window.h"
#include "StepManager.h"
#include "LogParser.h"
#include "FileDialog.h"

#include <imgui.h>
#include <string>

class Application
{
public:

	enum class State {
		FILE_DIALOG,
		ALL_EARS,
		NO_STONE_UNTURNED,
		SETTINGS
	};

	Application(int width, int height);
	~Application();

	void Run();
	void Update();

	void Save();
	void Load();

	void SetImGuiStyle();
	void RenderSettingsMenu();

private:
	bool running_ = true;
	bool moveable_ = false;
	int font_size_= 16;
	bool font_size_changed_ = false;

	std::vector<ImFont*> fonts_;

	Window window_;
	StepManager step_manager_;
	LogParser log_parser_;
	std::unique_ptr<FileDialog> file_dialog_;

	State state_ = State::ALL_EARS;

};