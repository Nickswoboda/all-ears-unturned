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

	Load();
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto-Medium.ttf", font_size_);

	ImGui_ImplGlfw_InitForOpenGL(window_.glfw_window_, true);
	ImGui_ImplOpenGL3_Init("#version 410");

	SetImGuiStyle();
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
				CheckStepCompletion();
			}
		}
		++frames;

		if (Window::IsFocused()) {
			Render();
		}

		glfwPollEvents();


		//Must change font between Render() and NewFrame()
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

void Application::CheckStepCompletion()
{

	if (all_ears_enabled_ && all_ears_manager_.StepIsComplete()) {
		all_ears_manager_.IncrementStep();
	}

	auto location = log_parser_.GetLocation();

	if (all_ears_enabled_ && all_ears_manager_.GetDestination() == location && all_ears_manager_.GetDestination() != "") {
		all_ears_manager_.IncrementStep();
	}

	if (no_stone_unturned_enabled_) {
		no_stone_manager_.ChangeLocation(location);
	}

	Render();

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

	std::vector<int> completed_lore;
	int id = 0;
	for (const auto& act : no_stone_manager_.acts_) {
		for (int i = 0; i < act.locations_.size(); ++i) {
			for (const auto& lore_item : act.lore_[i]) {
				if (lore_item.completed) {
					completed_lore.push_back(id);
				}

				++id;
			}
		}
	}
	json["completed lore"] = completed_lore;


	std::ofstream file("assets/save-info.json");
	file << std::setw(4) << json << std::endl;
	file.close();
}

void Application::Load()
{
	PushState(State::GUIDE);

	std::ifstream save_file("assets/save-info.json");
	if (!save_file.is_open()) {
		PushState(State::TUTORIAL);
		no_stone_manager_.LoadData(std::vector<int>());
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
	}
	else {
		PushState(State::LOAD_DATA_ERROR);
	}
	no_stone_manager_.LoadData(json["completed lore"]);
}

void Application::SetImGuiStyle()
{
	ImGuiStyle* style = &ImGui::GetStyle();

	style->WindowPadding = ImVec2(2, 2);

	style->WindowRounding = 0.0f;
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.2f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 0.0f, 0.0f, 0.8f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.8f);
	//style->Colors[ImGuiCol_Button] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.8f);

}

void Application::Render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if (window_.hidden_) {
		ImGui::SetNextWindowSize({ 100.0, (float)window_.height_ });
		ImGui::Begin("All Ears Unturned", &running_,
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

		ImVec2 pos = ImGui::GetWindowPos();
		if (pos.x != 0.0f || pos.y != 0.0f) {
			ImGui::SetWindowPos({ 0.0f, 0.0f });
			window_.Move(pos.x, pos.y);
		}
	}
	else {
		ImGui::SetNextWindowSize({ (float)window_.width_, (float)window_.height_ });
		if (moveable_) {
			ImGui::Begin("All Ears Unturned", &running_,
				ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

			ImVec2 pos = ImGui::GetWindowPos();
			if (pos.x != 0.0f || pos.y != 0.0f) {
				ImGui::SetWindowPos({ 0.0f, 0.0f });
				window_.Move(pos.x, pos.y);
			}
		}
		else {
			ImGui::Begin("All Ears Unturned", &running_,
				ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
		}

		ImGui::Separator();

		switch (state_stack_.top()) {
		case State::TUTORIAL:
			RenderTutorial();
			break;
		case State::LOAD_DATA_ERROR:
			RenderReadSaveFileError();
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
		if (state_stack_.top() != State::SETTINGS) {
			if (ImGui::Button("Settings")) {
				PushState(State::SETTINGS);
			}
		}
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
	ImGui::PushItemWidth(window_.width_ - 10);
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

	ImGui::Checkbox("Movable", &moveable_);
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

	if (ImGui::Button("Return")) {
		PopState();
	}

}

void Application::RenderReadSaveFileError()
{
	ImGui::TextWrapped("Unable to Read Save File");

	if (ImGui::Button("OK")) {
		PopState();
		PushState(State::FILE_DIALOG);
	}
}

void Application::RenderTutorial()
{
	ImGui::TextWrapped("Tutorial");
	ImGui::Separator();

	int num_pages = 3;
	switch (tutorial_page_) {
		case 1: 
			ImGui::TextWrapped("Follow the All Ears steps exactly as given. Do not enter new areas or complete quests. Doing so may cause you to miss certain dialog options.");
			ImGui::TextWrapped("NOTE: You MUST kill all Bandits in order to complete the acheivement.");
			break;
		case 2:
			ImGui::TextWrapped("Complete the objectives in the order given and mark off each checkbox when done. The step will automatically progress when all objectives are marked.");
			ImGui::TextWrapped("You can set the path of your Path of Exile folder in the settings menu. If set, the guide will progress automatically for steps that require travelling to areas");
			break;
		case 3:
			ImGui::TextWrapped("In the Settings menu, you may change the text size and width of the window. To minimize space, the window will change it's height based on it's contents.");
			ImGui::TextWrapped("You may also move the window by toggling the 'Movable' checkbox and dragging the window with your mouse.");
			ImGui::TextWrapped("You may reread this tutorial by clicking on the tutorial button in the settings menu at any time");
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
