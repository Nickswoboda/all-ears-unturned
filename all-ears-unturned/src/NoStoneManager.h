#pragma once

#include <vector>
#include <string>
#include "ActLore.h"
class NoStoneManager
{
public:

	NoStoneManager();

	void LoadData(std::vector<int> completed_lore);
	void Render();

	void ChangeLocation(const std::string& location);

	std::vector<ActLore> acts_;
	int max_items_ = 7;
	int current_act_ = 0;
	int current_location_ = 0;
};