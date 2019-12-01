#include "LogParser.h"

#include <fstream>
#include <sstream>

void LogParser::Init()
{
	std::ifstream file(log_file_path_, std::ios::in | std::ios::binary);
	if (file) {
		file.seekg(0, std::ios::end);
		end_of_log_ = file.tellg();
		file.close();
	}
	else {
		std::cout << "Could not open log file.";
	}
}

void LogParser::SetFolderPath(const std::string& path)
{
	folder_path_ = path;
	log_file_path_ = folder_path_ + "\\logs\\Client.txt";

	Init();
}

std::string LogParser::GetLocation()
{
	std::ifstream file(log_file_path_, std::ios::in);

	file.seekg(end_of_log_);
	std::stringstream buffer;
	buffer << file.rdbuf();

	file.seekg(0, std::ios::end);
	end_of_log_ = file.tellg();

	file.close();

	std::string log_text = buffer.str();

	if (log_text.find("You have entered") != std::string::npos) {
		auto pos = log_text.find("You have entered");
		std::string zone;

		for (int i = pos + 17; i < log_text.length() - 2; i++) {
			zone += log_text[i];
		}
		return zone;
	}

	return "";
}

