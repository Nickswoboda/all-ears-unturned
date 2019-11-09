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

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	fonts_.push_back(io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto-Medium.ttf", 16));

	ImGui_ImplGlfw_InitForOpenGL(window_.glfw_window_, true);
	ImGui_ImplOpenGL3_Init("#version 410");

	Load();

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
				window_.x_pos_ += pos.x;
				window_.y_pos_ += pos.y;
				ImGui::SetWindowPos({ 0.0f, 0.0f });
				window_.Update();
				std::cout << "window moved" << '\n';
			}
		}
		else {
			ImGui::Begin("All Ears Unturned", &running_,
				ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
		}

		ImGui::Separator();

		static int frames = 0;

		switch (state_) {
			case State::FILE_DIALOG:
				file_dialog_->Render();
				if (!file_dialog_->selected_path_.empty()) {
					log_parser_.log_file_path_ = file_dialog_->selected_path_;
					file_dialog_.release();
					state_ = State::ALL_EARS;
				}
				break;

			case State::ALL_EARS:
				if (frames > 10) {
					frames -= 10;
					Update();
				}
				++frames;
				
				step_manager_.Render();

				break;

			case State::SETTINGS:
				RenderSettingsMenu();
				break;
		}

		ImGui::Separator();
		if (ImGui::Button("Settings")) {
			state_ = State::SETTINGS;
		}

		if (window_.height_ != ImGui::GetCursorPosY()) {
			window_.height_ = ImGui::GetCursorPosY();
			window_.Update();
			std::cout << "window size changed" << '\n';
		}

		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window_.glfw_window_);
		glfwPollEvents();


		//only if font changed in setting menu
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
	if (step_manager_.StepIsComplete()) {
		step_manager_.IncrementStep();
	}

	if (step_manager_.GetDestination() != "") {
		if (log_parser_.HasEnteredLocation(step_manager_.GetDestination())) {
			step_manager_.IncrementStep();
		}
	}
}


void Application::Save()
{
	nlohmann::json json;
	json["log path"] = log_parser_.log_file_path_;
	json["current step"] = step_manager_.current_step_;
	json["window x"] = window_.x_pos_;
	json["window y"] = window_.y_pos_;

	std::ofstream file("assets/save-info.json");
	file << std::setw(4) << json << std::endl;
	file.close();
}

void Application::Load()
{
	std::ifstream save_file("assets/save-info.json");
	if (!save_file.is_open()) {
		return;
	}

	nlohmann::json save_json = nlohmann::json::parse(save_file);
	save_file.close();

	log_parser_.log_file_path_ = save_json["log path"];
	if (log_parser_.log_file_path_ == "") {
		state_ = State::FILE_DIALOG;
		file_dialog_ = std::make_unique<FileDialog>();
	}
	step_manager_.current_step_ = save_json["current step"];
	window_.x_pos_ = save_json["window x"];
	window_.y_pos_ = save_json["window y"];
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
		window_.Update();
	}

	if (ImGui::Button("movable")) {
		if (moveable_) {
			moveable_ = false;
		}
		else {
			moveable_ = true;
		}
	}

	if (ImGui::Button("Return")) {
		state_ = State::ALL_EARS;
	}
}
