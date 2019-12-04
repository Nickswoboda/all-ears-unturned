#pragma once

#include "Step.h"

#include <vector>
#include <memory>
class AllEarsManager
{
public:
	
	void LoadData();

	void Render();
	void IncrementStep();
	void DecrementStep();

	bool StepIsComplete(const std::string& location);

	std::vector<std::unique_ptr<Step>> steps_;
	int current_step_ = 0;
	bool achievement_complete_ = false;
};