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

	void SetCurrentStep(int step);

	bool StepIsComplete(const std::string& location);

	std::vector<std::unique_ptr<Step>> steps_;
	int current_step_ = 0;
	int num_dialogs_completed_ = 0;
	const int num_dialogs_total_ = 484;
	bool achievement_complete_ = false;
};
