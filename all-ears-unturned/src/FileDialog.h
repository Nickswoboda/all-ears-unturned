#pragma once

#include <string>
#include <stack>

class FileDialog
{
public:

	FileDialog();

	void Render();

	std::string current_file_path_ = "/";
	std::string selected_path_;
	std::stack<std::string> prev_paths_;

};