#include "FileDialog.h"

#include <imgui.h>

#include <filesystem>
#include <iostream>

#include <Windows.h>

FileDialog::FileDialog(int width)
	: width_(width)
{
	//get list of physical drives
	DWORD bufferlength = 100;
	char drive_buffer[100];
	DWORD string_length = GetLogicalDriveStrings(bufferlength, drive_buffer);
	if (string_length > 0 && string_length <= 100) {
		char* drive = drive_buffer;
		while (*drive) {
			drives_.push_back(drive);
			drive += strlen(drive_buffer) + 1;
		}
	}
}

void FileDialog::Render()
{
	ImGui::TextWrapped("Select the Path of Exile folder");
	ImGui::TextWrapped(current_file_path_.c_str());

	std::filesystem::path path = current_file_path_;

	if (prev_paths_.empty()) {
		ImGui::BeginChildFrame(1, { width_ - 5.0f, 300 });
		int index = 0;
		for (const auto& drive : drives_) {
			if (ImGui::Selectable(drive.c_str(), index_selected_ == index, ImGuiSelectableFlags_AllowDoubleClick)) {

				if (ImGui::IsMouseDoubleClicked(0)) {
					prev_paths_.push(drive);
					current_file_path_ = drive;
				}

				index_selected_ = index;
				selected_path_ = drive;
			}
			++index;
		}
		ImGui::EndChildFrame();
	}
	else {
		//try/catch needed for when choosing empty media device such as dvd player
		try {
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
		}
		catch (std::filesystem::filesystem_error & error) {};
	}

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

