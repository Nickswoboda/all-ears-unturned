#pragma once

#include <vector>
#include <string>

struct Lore
{
	std::string name_;
	bool completed_ = false;
};

struct LoreLocation
{
	std::string name_;
	std::vector<Lore> lore_;
	std::string note_;
};

struct LoreAct
{
	std::string name_;
	std::vector<LoreLocation> locations_;
};