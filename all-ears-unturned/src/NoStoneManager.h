#pragma once

#include "Lore.h"

#include <json.hpp>
#include <vector>
#include <string>
class NoStoneManager
{
public:

	void LoadData(const nlohmann::json& save_json = nlohmann::json());
	void Render();

	void ChangeLocation(const std::string& location);

	void Increment();
	void Decrement();

	bool CheckAreaCompletion() const;
	bool CheckAchievementCompletion() const;

	void Save(nlohmann::json& json);

	std::vector<LoreAct> acts_;
	LoreAct* current_act_ = nullptr;
	LoreLocation* current_location_ = nullptr;
	int max_items_per_page_ = 5;
	bool complete_ = false;
};