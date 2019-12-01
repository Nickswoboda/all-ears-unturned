#pragma once

#include <iostream>

class LogParser 
{
public:
	void Init();

	void SetFolderPath(const std::string& path);

	std::string GetLocation();
	
	std::string folder_path_;
	std::string log_file_path_;
	std::streamoff end_of_log_ = 0;
};