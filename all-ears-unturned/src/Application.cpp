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
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

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

		static int frames = 0;

		switch (state_stack_.top()) {
			case State::LOAD_DATA_ERROR:
				RenderReadSaveFileError();
				break;
			case State::FILE_DIALOG:
				file_dialog_->Render();
				if (file_dialog_->done_) {
					log_parser_.log_file_path_ = file_dialog_->full_log_path_;
					PopState();
				}
				break;

			case State::ALL_EARS:
				if (frames > 10) {
					frames -= 10;
					CheckStepCompletion();
				}
				++frames;

				if (show_progress_) {
					ImGui::ProgressBar((float)all_ears_manager_.current_step_ / all_ears_manager_.steps_.size());
					ImGui::Separator();
				}
				
				all_ears_manager_.Render();
				break;

			case State::NO_STONE_UNTURNED:
				no_stone_manager_.Render();
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
		if (window_.height_ != ImGui::GetCursorPosY()) {
			window_.ResizeHeight(ImGui::GetCursorPosY());
		}

		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window_.glfw_window_);
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
	if (all_ears_manager_.StepIsComplete()) {
		all_ears_manager_.IncrementStep();
	}

	if (all_ears_manager_.GetDestination() != "") {
		if (log_parser_.HasEnteredLocation(all_ears_manager_.GetDestination())) {
			all_ears_manager_.IncrementStep();
		}
	}
}


void Application::Save()
{
	nlohmann::json json;
	json["log path"] = log_parser_.log_file_path_;
	json["current step"] = all_ears_manager_.current_step_;
	json["window x"] = window_.x_pos_;
	json["window y"] = window_.y_pos_;
	json["window width"] = window_.width_;
	json["font size"] = font_size_;
	json["show progress"] = show_progress_;


	std::ofstream file("assets/save-info.json");
	file << std::setw(4) << json << std::endl;
	file.close();
}

void Application::Load()
{
	PushState(State::NO_STONE_UNTURNED);

	std::ifstream save_file("assets/save-info.json");
	if (!save_file.is_open()) {
		PushState(State::FILE_DIALOG);
		return;
	}

	nlohmann::json json = nlohmann::json::parse(save_file);

	save_file.close();

	
	if (json.count("log path") && json.count("current step") && json.count("window x") &&
		json.count("window y") && json.count("window width") && json.count("font size") &&
		json.count("show progress")) {

		log_parser_.log_file_path_ = json["log path"];
		all_ears_manager_.current_step_ = json["current step"];
		window_.Move(json["window x"], json["window y"]);
		window_.width_ = (json["window width"]);
		font_size_ = json["font size"];
		show_progress_ = json["show progress"];

		if (log_parser_.log_file_path_ == "") {
			PushState(State::FILE_DIALOG);
		}
	}
	else {
		PushState(State::LOAD_DATA_ERROR);
	}
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

void Application::RenderSettingsMenu()
{
	if (ImGui::DragInt("font size", &font_size_, 1.0f, 10, 32)) {
		font_size_changed_ = true;
	}
	if (ImGui::DragInt("window width", &window_.width_, 1.0f, 100, 1000)) {
		window_.UpdateSize();

		if (file_dialog_ != nullptr) {
			file_dialog_->width_ = window_.width_;
		}
	}

	ImGui::PushButtonRepeat(true);
	if (ImGui::Button("-")) {
		all_ears_manager_.DecrementStep();
	}
	ImGui::SameLine();
	if (ImGui::Button("+")) {
		all_ears_manager_.IncrementStep();
	}
	ImGui::PopButtonRepeat();
	ImGui::SameLine();
	ImGui::Text("Current Step: %d", all_ears_manager_.current_step_ + 1);

	ImGui::Checkbox("Movable", &moveable_);
	ImGui::Checkbox("Show Progress", &show_progress_);

	ImGui::TextWrapped("Current Log Folder:");
	ImGui::TextWrapped(log_parser_.log_file_path_.c_str());
	if (ImGui::Button("Change Folder Path")) {
		PushState(State::FILE_DIALOG);
	}

	if (ImGui::Button("Return")) {
		PopState();
	}

}

void Application::RenderReadSaveFileError()
{
	ImGui::Text("Unable to Read Save File");

	if (ImGui::Button("OK")) {
		PopState();
		PushState(State::FILE_DIALOG);
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
