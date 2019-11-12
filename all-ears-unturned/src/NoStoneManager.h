#pragma once

#include <vector>
#include <string>
class NoStoneManager
{
public:
	NoStoneManager();

	void Render();

	int max_items = 7;
	std::vector<std::string> list_;
};