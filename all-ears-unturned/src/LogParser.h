#pragma once

#include <iostream>

class LogParser 
{
public:
	void Init();

	std::string GetLocation();

	std::string log_file_path_;
	std::streamoff end_of_log_ = 0;
};