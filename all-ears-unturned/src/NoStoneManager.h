#pragma once

#include <vector>
#include <string>
#include "ActLore.h"
#include <array>
class NoStoneManager
{
public:
	NoStoneManager();

	void LoadData();
	void Render();

	std::array<ActLore, 11> acts_;
	int max_items_ = 7;
	int current_act_ = 0;
	int current_location_ = 0;
};