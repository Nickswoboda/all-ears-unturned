#pragma once

#include <string>
#include <vector>
#include <stack>
#include <filesystem>

class FileDialog
{
public:

	FileDialog(int width);

	void Render();

	int width_;
	int index_selected_ = -1;
	std::vector<std::string> drives_;
	std::string current_file_path_ = "/";
	std::stack<std::string> prev_paths_;
	std::filesystem::path selected_path_;
	std::string folder_path_;

	bool done_ = false;
};