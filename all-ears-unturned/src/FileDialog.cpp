#include "FileDialog.h"

#include <imgui.h>

#include <filesystem>
#include <iostream>


FileDialog::FileDialog(int width)
	: width_(width)
{
	auto path = std::filesystem::current_path();
	current_file_path_ = path.root_path().string();
	prev_paths_.push(current_file_path_);
}

void FileDialog::Render()
{
	ImGui::TextWrapped("Select the Path of Exile folder");
	ImGui::TextWrapped(current_file_path_.c_str());

	std::filesystem::path path = current_file_path_;
	if (!std::filesystem::exists(path)) {
		std::cout << "file path does not exist";
		return;
	}

	ImGui::BeginChildFrame(1, { width_ - 5.0f, 300 });
	int index = 0;
	for (auto& p : std::filesystem::directory_iterator(path, std::filesystem::directory_options::skip_permission_denied)) {
		if (p.is_directory()) {

			if (ImGui::Selectable(p.path().filename().u8string().c_str(), index_selected_ == index, ImGuiSelectableFlags_AllowDoubleClick)) {
				
				if (ImGui::IsMouseDoubleClicked(0)) {
					prev_paths_.push(current_file_path_);
					current_file_path_ = p.path().u8string();
				}

				index_selected_ = index;
				selected_path_ = p.path();
			}
		}

		++index;
	}
	ImGui::EndChildFrame();

	ImGui::TextWrapped("Selected:");
	ImGui::SameLine();
	ImGui::TextWrapped(selected_path_.filename().u8string().c_str());

	if (ImGui::Button("Prev")) {
		if (!prev_paths_.empty()) {
			current_file_path_ = prev_paths_.top();
			prev_paths_.pop();
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Select")) {
		if (!selected_path_.empty()) {
			folder_path_ = selected_path_.u8string();
			done_ = true;
		}
	}
}

