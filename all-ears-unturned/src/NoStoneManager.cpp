#include "NoStoneManager.h"

#include <imgui.h>
#include <string>
#include <fstream>
#include <json.hpp>

#include <iostream>

NoStoneManager::NoStoneManager()
{
	LoadData();
}

void NoStoneManager::Render()
{
	if (ImGui::BeginCombo("Act", acts_[current_act_].name_.c_str())) {
		for (int i = 0; i < acts_.size(); ++i) {
			if (ImGui::Selectable(acts_[i].name_.c_str(), current_act_ == i))
				current_act_ = i;
		}
		ImGui::EndCombo();
	}

	if (ImGui::BeginCombo("Location", acts_[current_act_].locations_[current_location_].c_str())) {
		for (int i = 0; i < acts_[current_act_].locations_.size(); ++i) {
			if (ImGui::Selectable(acts_[current_act_].locations_[i].c_str(), current_location_ == i))
				current_location_ = i;
		}
		ImGui::EndCombo();
	}
	ImGui::Separator();

	static bool bool_ = false;
	static int page_ = 1;
	int num_items = acts_[current_act_].lore_[current_location_].size();
	int num_pages = (num_items / 7) + 1;
	for (int i = (page_ - 1) * 7; i < 7 * page_ && i < num_items; ++i) {
		
		ImGui::Checkbox(acts_[current_act_].lore_[current_location_][i].c_str(), &bool_);
		++i;
	
		if (i < 7 * page_ && i < num_items) {
			ImGui::SameLine(333);
			ImGui::Checkbox(acts_[current_act_].lore_[current_location_][i].c_str(), &bool_);
		}
	}

	ImGui::Text("Page %d / %d", page_, num_pages);
	if (ImGui::Button("Next Page")) {
		++page_;
	}
}

void NoStoneManager::LoadData()
{
	std::ifstream file("assets/no-stone-unturned.json");
	if (!file.is_open()) {
		std::cout << "Error loading No Stone Unturned file";
	}

	nlohmann::json json = nlohmann::json::parse(file);

	file.close();

	int index = 0;
	for (const auto& act : json.items()) {
		acts_[index].name_ = act.key();

		for (const auto& location : act.value()) {
			std::string key = location.begin().key();
			acts_[index].locations_.push_back(key);
			std::vector<std::string> lore_temp;
			for (const auto& lore : location[key]) {
				lore_temp.push_back(lore);
			}
			acts_[index].lore_.push_back(lore_temp);
		}

		++index;
	}
}
