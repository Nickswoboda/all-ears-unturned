#pragma once

#include <vector>
#include <string>

struct Lore
{
	std::string name;
	bool completed = false;

};

class ActLore
{
public:


	ActLore() = default;

	std::string name_;
	std::vector<std::string> locations_;
	std::vector<std::vector<Lore>> lore_;
	
};