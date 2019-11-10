#pragma once

#include <string>
#include <stack>

class FileDialog
{
public:

	FileDialog(int width);

	void Render();

	int width_;
	std::string current_file_path_ = "/";
	std::string selected_path_;
	std::stack<std::string> prev_paths_;

};