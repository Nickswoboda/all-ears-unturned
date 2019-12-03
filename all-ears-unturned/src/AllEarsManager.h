#pragma once

#include "Step.h"

#include <vector>
#include <memory>
class AllEarsManager
{
public:
	AllEarsManager();

	void Render();
	void IncrementStep();
	void DecrementStep();

	bool StepIsComplete(const std::string& location);

	std::vector<std::unique_ptr<Step>> steps_;
	int current_step_ = 1;
	bool achievement_complete_ = false;
};