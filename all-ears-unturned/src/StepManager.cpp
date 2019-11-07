#include "StepManager.h"

#include "Step.h"

#include <json.hpp>
#include <imgui.h>

#include <fstream>

StepManager::StepManager()
{
	std::ifstream file("assets/steps.json");

	nlohmann::json json = nlohmann::json::parse(file);

	for (const auto& index : json) {
		std::string key(index.begin().key());

		if (key == "NPC") {

			std::vector<std::string> dialogs;
			for (const auto& subject : index["subjects"]) {
				dialogs.push_back(subject);
			}

			steps_.push_back(std::make_unique<NpcStep>(index["NPC"], dialogs ));
		}
		else if (key == "destination") {
			steps_.push_back(std::make_unique<TravelStep>(index["destination"]));
		}
		else if (key == "event") {
			steps_.push_back(std::make_unique<EventStep>(index["event"]));
		}
		else {
			std::cout << "JSON syntax error";
		}
	}
}

void StepManager::Render()
{
	steps_[current_step_]->Render();

	ImGui::SetCursorPos({ 10, 175 });
	if (ImGui::Button("Prev")) {
		DecrementStep();
	}
	ImGui::SameLine(50);
	if (ImGui::Button("Next")) {
		IncrementStep();
	}
}

void StepManager::IncrementStep()
{
	if ((current_step_ + 1) < steps_.size()) {

		//reset dialog checkboxes
		auto npc_step = dynamic_cast<NpcStep*>(steps_[current_step_].get());
		if (npc_step != nullptr) {
			for (auto& dialog : npc_step->dialogs_) {
				dialog.completed = false;
			}
		}

		++current_step_;
	}
	else {
		std::cout << "AT END OF STEPS";
	}
}

void StepManager::DecrementStep()
{
	if (current_step_ > 0) {
		--current_step_;
	}
	else {
		std::cout << "AT THE BEGINNING OF STEPS";
	}
}

std::string StepManager::GetDestination()
{
	auto travel_step = dynamic_cast<TravelStep*>(steps_[current_step_].get());
	if (travel_step != nullptr) {
		return travel_step->destination_;
	}

	return "";
}

bool StepManager::StepIsComplete()
{
	auto npc_step = dynamic_cast<NpcStep*>(steps_[current_step_].get());
	if (npc_step != nullptr) {
		bool all_dialog_complete = true;

		for (const auto& dialog : npc_step->dialogs_) {
			if (!dialog.completed) {
				all_dialog_complete = false;
				break;
			}
		}
		if (all_dialog_complete) {
			return true;
		}
	}

	auto event_step = dynamic_cast<EventStep*>(steps_[current_step_].get());
	if (event_step != nullptr) {

		if (event_step->completed_) {
			event_step->completed_ = false;
			return true;
		}
	}
	return false;
}
