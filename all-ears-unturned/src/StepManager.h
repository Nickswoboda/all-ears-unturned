#pragma once

#include "Step.h"

#include <vector>
#include <memory>
class StepManager
{
public:
	StepManager();

	void Render();
	void IncrementStep();
	void DecrementStep();

private:
	std::vector<std::unique_ptr<Step>> steps_;
	int current_step_ = 0;
};