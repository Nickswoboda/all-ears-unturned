#pragma once

#include <iostream>

class LogParser 
{
public:
	void Init();

	bool HasEnteredLocation(const std::string& location);

	std::string log_file_path_;
	std::streamoff end_of_log_ = 0;
};