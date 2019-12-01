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

	void Advance();

	bool CheckAreaCompletion();
	void CheckAchievementCompletion();

	std::vector<ActLore> acts_;
	int max_items_ = 5;
	int current_act_ = 0;
	int current_location_ = 0;
	bool complete_ = false;
};