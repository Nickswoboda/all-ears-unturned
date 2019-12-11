#include "Application.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#include <json.hpp>

#include <iostream>
#include <iomanip>
#include <fstream>

Application::Application(int width, int height)
	:window_(width, height)
{
	if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress))) {
		std::cout << "could not load GLAD";
	}

	PushState(State::GUIDE);
	Load();
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	if (std::filesystem::exists("assets/fonts/Roboto-Medium.ttf")) {
		io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto-Medium.ttf", font_size_);
	}

	ImGui_ImplGlfw_InitForOpenGL(window_.glfw_window_, true);
	ImGui_ImplOpenGL3_Init("#version 410");

	SetImGuiStyle();

	if (!AssetsExist()) {
		PushState(State::LOAD_DATA_ERROR);
		error_message_ = "Unable to load one or more asset files.";
	}
	
	glfwSetWindowUserPointer(window_.glfw_window_, this);
	SetKeyCallbacks();
}

Application::~Application()
{
	Save();
	glfwTerminate();
}

void Application::Run()
{
	while (!glfwWindowShouldClose(window_.glfw_window_) && running_)
	{
		static int frames = 0;
		if (frames > 20) {
			frames = 0;
			if (state_stack_.top() == State::GUIDE) {
				Update();
			}
		}
		++frames;

		if (Window::IsFocused()) {
			Render();
		}

		glfwPollEvents();

		//Must change font outside of ImGui Rendering
		if (font_size_changed_) {
			font_size_changed_ = false;

			ImGuiIO& io = ImGui::GetIO();
			delete io.Fonts;
			io.Fonts = new ImFontAtlas();
			io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto-Medium.ttf", font_size_);
			ImGui_ImplOpenGL3_CreateFontsTexture();
		}
	}

}

void Application::Update()
{
	auto location = log_parser_.GetLocation();

	if (all_ears_enabled_ && all_ears_manager_.StepIsComplete(location)) {
		all_ears_manager_.IncrementStep();
	}

	//if (no_stone_unturned_enabled_ && location != "") {
	//	no_stone_manager_.ChangeLocation(location);
	//}

	if (!Window::IsFocused()) {
		Render();
	}
}

void Application::Render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if (window_.collapsed_) {
		ImGui::SetNextWindowSize({ 100.0, (float)window_.height_ });
		ImGui::Begin("All Ears Unturned", &running_, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);
	}
	else {
		ImGui::SetNextWindowSize({ (float)window_.width_, (float)window_.height_ });
		if (window_.movable_) {
			ImGui::Begin("All Ears Unturned", &running_, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);
		}
		else {
			ImGui::Begin("All Ears Unturned", &running_, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove);
		}

		ImGui::Separator();

		switch (state_stack_.top()) {
			case State::TUTORIAL:
				RenderTutorial();
				break;

			case State::LOAD_DATA_ERROR:
				RenderErrorMessage();
				break;

			case State::FILE_DIALOG:
				file_dialog_->Render();
				if (file_dialog_->done_) {
					log_parser_.SetFolderPath(file_dialog_->folder_path_);
					PopState();
				}
				break;

			case State::GUIDE:
				if (all_ears_enabled_) {
					all_ears_manager_.Render();
					ImGui::Separator();
				}
				if (no_stone_unturned_enabled_) {
					no_stone_manager_.Render();
				}
				break;

			case State::SETTINGS:
				RenderSettingsMenu();
				break;
		}

		ImGui::Separator();
		if (state_stack_.top() != State::SETTINGS && state_stack_.top() != State::LOAD_DATA_ERROR) {
			if (ImGui::Button("Settings")) {
				//used to avoid being able infinitely stack states
				if (state_stack_.top() == State::FILE_DIALOG || state_stack_.top() == State::TUTORIAL) {
					PopState();
				}
				else {
					PushState(State::SETTINGS);
				}
			}
		}
	}

	ImVec2 pos = ImGui::GetWindowPos();
	if (pos.x != 0.0f || pos.y != 0.0f) {
		ImGui::SetWindowPos({ 0.0f, 0.0f });
		window_.Move(pos.x, pos.y);
	}

	if (window_.height_ != ImGui::GetCursorPosY()) {
		window_.ResizeHeight(ImGui::GetCursorPosY());
	}
	
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(window_.glfw_window_);
}

void Application::RenderSettingsMenu()
{
	ImGui::TextWrapped("Font Size");
	ImGui::PushItemWidth(window_.width_);
	if (ImGui::DragInt("##font", &font_size_, 1.0f, 10, 32)) {
		font_size_changed_ = true;
	}
	ImGui::TextWrapped("Window Width");
	if (ImGui::DragInt("##width", &window_.width_, 1.0f, 100, 1000)) {
		window_.UpdateSize();

		if (file_dialog_ != nullptr) {
			file_dialog_->width_ = window_.width_;
		}
	}
	ImGui::PopItemWidth();


	ImGui::TextWrapped("Change Step");
	ImGui::PushButtonRepeat(true);
	ImGui::SameLine();
	int step = all_ears_manager_.current_step_ + 1;
	if (ImGui::InputInt("##step", &step)) {
		if (step > 0 && step < all_ears_manager_.steps_.size()) {
			all_ears_manager_.current_step_ = step - 1;
		}
	}
	ImGui::PopButtonRepeat();

	ImGui::Checkbox("Movable", &window_.movable_);
	ImGui::Checkbox("All Ears", &all_ears_enabled_);
	ImGui::Checkbox("No Stone Unturned", &no_stone_unturned_enabled_);

	ImGui::TextWrapped("Current Log Folder:");
	ImGui::TextWrapped(log_parser_.folder_path_.c_str());
	if (ImGui::Button("Change Folder Path")) {
		PushState(State::FILE_DIALOG);
	}

	if (ImGui::Button("Tutorial")) {
		PushState(State::TUTORIAL);
	}
	if (ImGui::Button("Save")) {
		Save();
	}


	if (ImGui::Button("Return")) {
		PopState();
	}

}

void Application::RenderTutorial()
{
	ImGui::TextWrapped("Tutorial");
	ImGui::Separator();

	int num_pages = 4;
	switch (tutorial_page_) {
		case 1: 
			ImGui::Bullet();
			ImGui::TextWrapped("Follow the All Ears steps exactly as given. Do not enter new areas or complete quests. Doing so may cause you to miss certain dialog options.");
			ImGui::Bullet();
			ImGui::TextWrapped("NOTE: You MUST kill all Bandits in order to complete the acheivement.");
			break;
		case 2:
			ImGui::Bullet();
			ImGui::TextWrapped("Complete the objectives in the order given and mark off each checkbox when done. The step will automatically progress when all objectives are marked.");
			ImGui::Bullet();
			ImGui::TextWrapped("You can set the path of your Path of Exile folder in the settings menu. If set, the guide will progress automatically for steps that require travelling to areas.");
			ImGui::Bullet();
			ImGui::TextWrapped("You may turn off the tracking of either achievement in the Settings menu.");
			break;
		case 3:
			ImGui::Bullet();
			ImGui::TextWrapped("After each Act there will be a note stating how many dialogs out of the 509 you should have completed up to that point. You may check the Achievements tab in-game to see if they match up.");
			ImGui::Bullet();
			ImGui::TextWrapped("If not, that means you have missed one or more of the required dialogue options. You must make a new character and try again if you would like to complete the achievement");
			ImGui::Bullet();
			ImGui::TextWrapped("If you believe that you followed the steps exactly as given, please post a message on GitHub or the Reddit threads, so any errors may be found and fixed. Thank you.");
			break;
		case 4:
			ImGui::Bullet();
			ImGui::TextWrapped("This window will block input from PoE. If needed, you may press the 'E' key to collapse and move the window out of the way. Pressing the 'E' key again will expand the window back to normal");
			ImGui::Bullet();
			ImGui::TextWrapped("You may press 'Q' and 'W' to go backwards and forwards for the 'All Ears' steps. You may press 'A' and 'S' to go backwards and forwards for the 'No Stone Unturned' steps.");
			ImGui::Bullet();
			ImGui::TextWrapped("In the Settings menu, you may change the text size and width of the window. To minimize space, the window will change it's height based on it's contents.");
			ImGui::Bullet();
			ImGui::TextWrapped("You may also move the window by toggling the 'Movable' checkbox and dragging the window with your mouse.");
			ImGui::Bullet();
			ImGui::TextWrapped("You may reread this tutorial by clicking on the tutorial button in the settings menu at any time.");
	}

	if (ImGui::ArrowButton("Left", ImGuiDir_Left) && tutorial_page_ > 1) {
		--tutorial_page_;
	}
	ImGui::SameLine();
	ImGui::Text("%d / %d", tutorial_page_, num_pages);
	ImGui::SameLine();
	if (ImGui::ArrowButton("Right", ImGuiDir_Right) && tutorial_page_ < num_pages) {
		++tutorial_page_;
	}

	if (ImGui::Button("Okay")) {
		PopState();
	}
	
}

void Application::RenderErrorMessage()
{
	ImGui::TextWrapped(error_message_.c_str());

	if (ImGui::Button("OK") && AssetsExist()) {
		PopState();
	}
}

void Application::PushState(State state)
{
	state_stack_.push(state);

	if (state == State::FILE_DIALOG) {
		file_dialog_ = std::make_unique<FileDialog>(window_.width_);
	}
}

void Application::PopState()
{
	if (state_stack_.top() == State::FILE_DIALOG) {
		file_dialog_.release();
	}

	state_stack_.pop();
}

void Application::SetImGuiStyle()
{
	ImGuiStyle* style = &ImGui::GetStyle();

	style->WindowPadding = ImVec2(2, 2);

	style->WindowRounding = 0.0f;
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.2f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 0.0f, 0.0f, 0.8f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.8f);
}

void Application::Load()
{
	all_ears_manager_.LoadData();
	std::ifstream save_file("assets/save-info.json");
	if (!save_file.is_open()) {
		PushState(State::TUTORIAL);
		no_stone_manager_.LoadData();
		
		return;
	}

	nlohmann::json json = nlohmann::json::parse(save_file);
	save_file.close();

	
	if (json.count("log folder path") && json.count("current step") && json.count("window x") &&
		json.count("window y") && json.count("window width") && json.count("font size") && 
		json.count("no stone unturned enabled") && json.count("all ears enabled")) {

		log_parser_.SetFolderPath(json["log folder path"]);
		all_ears_manager_.current_step_ = json["current step"];
		window_.Move(json["window x"], json["window y"]);
		window_.width_ = (json["window width"]);
		font_size_ = json["font size"];
		all_ears_enabled_ = json["all ears enabled"];
		no_stone_unturned_enabled_ = json["no stone unturned enabled"];
		no_stone_manager_.LoadData(json);
	}
	else {
		PushState(State::LOAD_DATA_ERROR);
		error_message_ = "No save data was found. Settings will return to their default values.";
	}
}

bool Application::AssetsExist()
{
	if (std::filesystem::exists("assets/fonts/Roboto-Medium.ttf") && std::filesystem::exists("assets/steps.json") && std::filesystem::exists("assets/no-stone-unturned.json")) {
		return true;
	}

	return false;
}

void Application::Save()
{
	nlohmann::json json;
	json["log folder path"] = log_parser_.folder_path_;
	json["current step"] = all_ears_manager_.current_step_;
	json["window x"] = window_.x_pos_;
	json["window y"] = window_.y_pos_;
	json["window width"] = window_.width_;
	json["font size"] = font_size_;
	json["all ears enabled"] = all_ears_enabled_;
	json["no stone unturned enabled"] = no_stone_unturned_enabled_;

	no_stone_manager_.Save(json);

	std::ofstream file("assets/save-info.json");
	file << std::setw(4) << json << std::endl;
	file.close();
}

void Application::SetKeyCallbacks()
{
	glfwSetKeyCallback(window_.glfw_window_, [](GLFWwindow* window, int key, int scancode, int action, int mods) {

		auto* app = (Application*)glfwGetWindowUserPointer(Window::glfw_window_);
		if (key == GLFW_KEY_E && action == GLFW_PRESS) {
			if (!app->window_.collapsed_) {
				app->window_.collapsed_ = true;
			}
			else {
				app->window_.collapsed_ = false;
			}
		}
		if (app->all_ears_enabled_) {
			if (key == GLFW_KEY_Q && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
				app->all_ears_manager_.DecrementStep();
			}
			if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
				app->all_ears_manager_.IncrementStep();
			}
		}
		if (app->no_stone_unturned_enabled_) {
			if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
				app->no_stone_manager_.Decrement();
			}
			if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
				app->no_stone_manager_.Increment();
			}
		}
	});
}


