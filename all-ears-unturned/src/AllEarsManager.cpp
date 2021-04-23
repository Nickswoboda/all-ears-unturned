#include "AllEarsManager.h"

#include "Step.h"

#include <json.hpp>
#include <imgui.h>

#include <fstream>

void AllEarsManager::LoadData()
{
	std::ifstream file("assets/steps.json");

	nlohmann::json json = nlohmann::json::parse(file);

	for (const auto& index : json) {
		std::string key(index.begin().key());

		if (key == "NPC") {
			steps_.push_back(std::make_unique<NpcStep>(index));
		}
		else if (key == "destination") {
			steps_.push_back(std::make_unique<TravelStep>(index));
		}
		else if (key == "event") {
			steps_.push_back(std::make_unique<EventStep>(index));;
		}
		else {
			std::cout << "JSON syntax error";
		}
	}
}

void AllEarsManager::Render()
{
	ImGui::Text("ALL EARS");
	ImGui::Separator();

	auto npc_step = dynamic_cast<NpcStep*>(steps_[current_step_].get());
	if (npc_step){
		ImGui::Text("Progress: %d / %d", num_dialogs_completed_ + npc_step->dialogs_completed_, 505);
	} else {
		ImGui::Text("Progress: %d / %d", num_dialogs_completed_, 505);
	}
	ImGui::Separator();

	if (ImGui::ArrowButton("##left", ImGuiDir_Left)) {
		DecrementStep();
	}
	ImGui::SameLine();

	if (!achievement_complete_) {
		ImGui::Text("Step: %d / %d", current_step_ + 1, steps_.size());

		ImGui::SameLine();
		if (ImGui::ArrowButton("##right", ImGuiDir_Right)) {
			IncrementStep();
		}
	}
	ImGui::Separator();
	if (achievement_complete_) {
		ImGui::TextWrapped("You have completed the All Ears Achievement!");
	}

	else {
		steps_[current_step_]->Render();
	}

}

void AllEarsManager::IncrementStep()
{
	if (achievement_complete_) return;
	//reset dialog checkboxes
	auto npc_step = dynamic_cast<NpcStep*>(steps_[current_step_].get());
	if (npc_step != nullptr) {
		for (auto& dialog : npc_step->dialogs_) {
			dialog.completed_ = false;
		}

		num_dialogs_completed_ += npc_step->dialogs_.size();
		npc_step->dialogs_completed_ = 0;
	}
	//reset dialog checkboxes
	auto event_step = dynamic_cast<EventStep*>(steps_[current_step_].get());
	if (event_step != nullptr) {
		for (auto& event : event_step->events_) {
			event.completed_ = false;
		}
	}

	if ((current_step_ + 1) < steps_.size()) {
		++current_step_;
	}
	else {
		achievement_complete_ = true;
	}
}

void AllEarsManager::DecrementStep()
{
	if (current_step_ == 0) return;

	if (achievement_complete_) {
		achievement_complete_ = false;
	}
	else {
		--current_step_;
	}
	auto npc_step = dynamic_cast<NpcStep*>(steps_[current_step_].get());
	if (npc_step) {
		num_dialogs_completed_ -= npc_step->dialogs_.size();
	}
}

bool AllEarsManager::StepIsComplete(const std::string& location)
{
	auto npc_step = dynamic_cast<NpcStep*>(steps_[current_step_].get());
	if (npc_step != nullptr) {
		bool all_dialog_complete = true;

		for (const auto& dialog : npc_step->dialogs_) {
			if (!dialog.completed_) {
				all_dialog_complete = false;
				break;
			}
		}
		if (all_dialog_complete) {
			return true;
		}
	}

	auto travel_step = dynamic_cast<TravelStep*>(steps_[current_step_].get());
	if (travel_step != nullptr) {
		if (travel_step->destination_ == location) {
			return true;
		}
		return false;
	}

	auto event_step = dynamic_cast<EventStep*>(steps_[current_step_].get());
	if (event_step != nullptr) {
		bool all_events_complete = true;
		for (const auto& event : event_step->events_) {
			if (!event.completed_) {
				all_events_complete = false;
				break;
			}
		}
		if (all_events_complete) {
			return true;
		}
	}
	return false;
}

void AllEarsManager::SetCurrentStep(int step)
{
	current_step_ = step;

	num_dialogs_completed_ = 0;
	for (int i = 0; i < current_step_; ++i) {
		auto npc_step = dynamic_cast<NpcStep*>(steps_[i].get());
		if (npc_step) {
			num_dialogs_completed_ += npc_step->dialogs_.size();
		}
	}
}
