#pragma once

#include <iostream>

class LogParser 
{
public:
	void Init();

	void SetLogPath(const std::string& path);

	std::string GetLocation();
	

	std::string log_file_path_;
	std::streamoff end_of_log_ = 0;
};