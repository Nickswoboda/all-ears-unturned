#include "FileDialog.h"

#include <imgui.h>

#include <filesystem>
#include <iostream>


FileDialog::FileDialog()
{
	auto path = std::filesystem::current_path();
	current_file_path_ = path.root_path().string();
	prev_paths_.push(current_file_path_);
}

void FileDialog::Render()
{
	ImGui::Begin("File System");
	ImGui::Text(current_file_path_.c_str());

	std::filesystem::path path = current_file_path_;
	if (!std::filesystem::exists(path)) {
		std::cout << "file path does not exist";
	}
	for (auto& p : std::filesystem::directory_iterator(path, std::filesystem::directory_options::skip_permission_denied)) {
		if (p.is_directory()) {
			if (ImGui::Button(p.path().u8string().c_str())) {
				prev_paths_.push(current_file_path_);
				current_file_path_ = p.path().u8string();
			}
		}
		else if (p.path().extension() == ".txt") {
			if (ImGui::Button(p.path().string().c_str())) {
				selected_path_ = p.path().u8string();
			}
		}
	}

	if (ImGui::Button("Back")) {
		if (!prev_paths_.empty()) {
			current_file_path_ = prev_paths_.top();
			prev_paths_.pop();
		}
	}
	ImGui::End();
}

