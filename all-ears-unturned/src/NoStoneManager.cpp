#include "NoStoneManager.h"

#include "Window.h"

#include <imgui.h>
#include <string>
#include <fstream>
#include <json.hpp>

#include <iostream>

NoStoneManager::NoStoneManager()
{
}

void NoStoneManager::Render()
{

	ImGui::Separator();
	ImGui::Separator();
	ImGui::Separator();
	ImGui::Text("NO STONE UNTURNED");
	ImGui::Separator();

	if (complete_) {
		ImGui::TextWrapped("You have completed the No Stone Unturned achievement!");

		if (ImGui::Button("Back")) {
			complete_ = false;
		}
		return;
	}
	
	if (ImGui::BeginCombo("Act", current_act_->name_.c_str())) {
		for (auto& act : acts_) {
			if (ImGui::Selectable(act.name_.c_str())) {
				current_act_ = &act;
				current_location_ = &act.locations_[0];
			}
		}
		//selectable popup does not close if user clicks out of window and loses focus
		//must do manually
		if (!Window::IsFocused()) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndCombo();
	}

	if (ImGui::BeginCombo("Location", current_location_->name_.c_str())) {
		if (Window::IsFocused()) {
			for (auto& location : current_act_->locations_) {
				if (ImGui::Selectable(location.name_.c_str())) {
					current_location_ = &location;
				}
			}
		}
	
		if (!Window::IsFocused()) {
			ImGui::CloseCurrentPopup();
		}
	
		ImGui::EndCombo();
	}
	
	ImGui::Separator();

	int num_pages = 1;
	static int current_page = 1;
	int num_items = current_location_->lore_.size();
	if (num_items > max_items_per_page_) {
		if (num_items % max_items_per_page_ == 0) {
			num_pages = num_items / max_items_per_page_;
		}
		else {
			num_pages = (num_items / max_items_per_page_) + 1;
		}
	}
	else {
		current_page = 1;
	}
	
	
	for (int i = (current_page - 1) * max_items_per_page_; (i < num_items) && i < (current_page * max_items_per_page_); ++i) {
		auto& current_lore = current_location_->lore_[i];
		if (ImGui::Checkbox(current_lore.name_.c_str(), &current_lore.completed_) && current_lore.completed_) {
			if (CheckAreaCompletion()) {
				current_page = 1;
				Advance();
				CheckAchievementCompletion();
				break;
			}
		}
	}
	
	if (num_pages > 1) {
		if (ImGui::Button("Prev")) {
			if (current_page > 1) {
				--current_page;
			}
		}
	
		ImGui::SameLine();
		ImGui::Text("%d / %d", current_page, num_pages);
		ImGui::SameLine();
	
		if (ImGui::Button("Next")) {
			if (current_page < num_pages) {
				++current_page;
			}
		}
	}
}

void NoStoneManager::ChangeLocation(const std::string& location)
{

	for (auto& act : acts_) {
		for (auto& area : act.locations_) {
			if (area.name_ == location) {
				current_act_ = &act;
				current_location_ = &area;
			}
		}
	}
}

void NoStoneManager::Advance()
{
	
	if (current_location_ != &current_act_->locations_.back()) {
		++current_location_;
	}
	else if (current_act_ != &acts_.back()){
		++current_act_;
		current_location_ = &current_act_->locations_[0];
	}
}

bool NoStoneManager::CheckAreaCompletion()
{
	for (const auto& lore_item : current_location_->lore_) {
		if (!lore_item.completed_) {
			return false;
		}
	}
	return true;
}

void NoStoneManager::CheckAchievementCompletion()
{
	for (const auto& act : acts_) {
		for (const auto& location : act.locations_) {
			for (const auto& lore : location.lore_) {
				if (!lore.completed_) {
					return;
				}
			}
		}
	}

	complete_ = true;
}

void NoStoneManager::LoadData(std::vector<int> completed_lore)
{
	std::ifstream file("assets/no-stone-unturned.json");
	if (!file.is_open()) {
		std::cout << "Error loading No Stone Unturned file";
	}
	nlohmann::json json;

	file >> json;

	file.close();

	int id = 0;
	for (const auto& act : json) {

		auto key = act.begin().key();
		LoreAct new_act{ key };

		for (const auto& location : act[key]) {

			LoreLocation new_location{ location.begin().key() };

			for (const auto& lore : location[location.begin().key()]) {
				bool completed = false;

				for (const auto& item : completed_lore) {
					if (id == item) {
						completed = true;
						break;
					}
				}
				new_location.lore_.push_back(Lore{ lore, completed });
				++id;
			}
			new_act.locations_.push_back(new_location);
		}

		acts_.push_back(new_act);
	}

	current_act_ = &acts_[0];
	current_location_ = &current_act_->locations_[0];
}
