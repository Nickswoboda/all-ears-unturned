#pragma once

#include <string>
#include <stack>
#include <filesystem>

class FileDialog
{
public:

	FileDialog(int width);

	void Render();

	int width_;
	int index_selected_ = -1;
	std::string current_file_path_ = "/";
	std::filesystem::path selected_path_;
	std::string full_log_path_;
	bool done_ = false;
	std::stack<std::string> prev_paths_;

};