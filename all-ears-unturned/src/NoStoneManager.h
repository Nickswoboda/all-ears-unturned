#pragma once

#include <vector>
#include <string>
#include "Lore.h"
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

	std::vector<LoreAct> acts_;
	LoreAct* current_act_ = nullptr;
	LoreLocation* current_location_ = nullptr;
	int max_items_per_page_ = 5;
	bool complete_ = false;
};