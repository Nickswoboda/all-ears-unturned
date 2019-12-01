#pragma once

#include "Window.h"
#include "AllEarsManager.h"
#include "NoStoneManager.h"
#include "LogParser.h"
#include "FileDialog.h"

#include <imgui.h>
#include <string>

class Application
{
public:

	enum class State {
		TUTORIAL,
		FILE_DIALOG,
		GUIDE,
		SETTINGS,
		LOAD_DATA_ERROR
	};

	Application(int width, int height);
	~Application();

	void Run();
	void CheckStepCompletion();

	void Save();
	void Load();

	void SetImGuiStyle();
	void RenderSettingsMenu();
	void RenderReadSaveFileError();
	void RenderTutorial();

	void PushState(State state);
	void PopState();

private:
	bool running_ = true;
	bool moveable_ = true;
	int font_size_= 16;
	bool font_size_changed_ = false;
	bool all_ears_enabled_ = true;
	bool no_stone_unturned_enabled_ = true;
	int tutorial_page_ = 1;
	bool hidden_ = false;

	Window window_;
	AllEarsManager all_ears_manager_;
	NoStoneManager no_stone_manager_;
	LogParser log_parser_;
	std::unique_ptr<FileDialog> file_dialog_;

	std::stack<State> state_stack_;
};