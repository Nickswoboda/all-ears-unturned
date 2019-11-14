#pragma once

#include <vector>
#include <string>

class ActLore
{
public:
	ActLore() = default;

	std::string name_;
	std::vector<std::string> locations_;
	std::vector < std::vector<std::string>> lore_;
	
};