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

Application::Application()
{
	if (!glfwInit()) {
		std::cout << "Could not initialize GLFW";
	}

	window_ = glfwCreateWindow(640, 480, "All Ears Unturned", NULL, NULL);
	if (!window_){
		glfwTerminate();
		std::cout << "Could not create window";
	}

	glfwMakeContextCurrent(window_);

	if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress))) {
		std::cout << "could not load GLAD";
	}

	
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window_, true);
	ImGui_ImplOpenGL3_Init("#version 410");

	Load();
}

Application::~Application()
{
	Save();
	glfwTerminate();
}

void Application::Run()
{
	while (!glfwWindowShouldClose(window_))
	{
		Update();

		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		step_manager_.Render();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window_);
		glfwPollEvents();
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

	std::ofstream file("assets/jsons/save-info.json");
	file << std::setw(4) << json << std::endl;
}

void Application::Load()
{
	std::ifstream save_file("assets/save-info.json");
	if (!save_file.is_open()) {
		return;
	}

	nlohmann::json save_json = nlohmann::json::parse(save_file);
	log_parser_.log_file_path_ = save_json["log path"];
	step_manager_.current_step_ = save_json["current step"];
}
