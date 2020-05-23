#include "NoStoneManager.h"

#include "Window.h"

#include <imgui.h>
#include <string>
#include <fstream>
#include <json.hpp>

#include <iostream>

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

	static int current_page = 1;

	if (ImGui::ArrowButton("##nsleft", ImGuiDir_Left)) {
		Decrement();
		current_page = 1;
	}

	ImGui::SameLine();
	if (ImGui::ArrowButton("##nsright", ImGuiDir_Right)) {
		Increment();
		current_page = 1;
	}

	ImGui::Separator();

	int num_pages = 1;
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
		auto id = "##" + current_lore.name_;
		if (ImGui::Checkbox(id.c_str(), &current_lore.completed_) && current_lore.completed_) {
			if (CheckAreaCompletion()) {
				Increment();
				current_page = 1;
				complete_ = CheckAchievementCompletion();
				break;
			}
		}
		ImGui::SameLine();
		ImGui::TextWrapped(current_lore.name_.c_str());
	}

	if (!current_location_->note_.empty()) {
		ImGui::PushStyleColor(ImGuiCol_Text, { 1.0, 0.0, 0.0, 1.0 });
		ImGui::TextWrapped(current_location_->note_.c_str());
		ImGui::PopStyleColor();
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

void NoStoneManager::Increment()
{
	
	if (current_location_ != &current_act_->locations_.back()) {
		++current_location_;
	}
	else if (current_act_ != &acts_.back()){
		++current_act_;
		current_location_ = &current_act_->locations_.front();
	}
}

void NoStoneManager::Decrement()
{
	if (current_location_ != &current_act_->locations_.front()) {
		--current_location_;
	}
	else if (current_act_ != &acts_.front()) {
		--current_act_;
		current_location_ = &current_act_->locations_.back();
	}
}

bool NoStoneManager::CheckAreaCompletion() const
{
	for (const auto& lore_item : current_location_->lore_) {
		if (!lore_item.completed_) {
			return false;
		}
	}
	return true;
}

bool NoStoneManager::CheckAchievementCompletion() const
{
	for (const auto& act : acts_) {
		for (const auto& location : act.locations_) {
			for (const auto& lore : location.lore_) {
				if (!lore.completed_) {
					return false;
				}
			}
		}
	}

	return true;
}

void NoStoneManager::Save(nlohmann::json& json)
{
	std::vector<int> completed_lore;
	int id = 0;

	for (const auto& act : acts_) {
		for (const auto& location : act.locations_) {
			for (const auto& lore : location.lore_) {
				if (lore.completed_) {
					completed_lore.push_back(id);
				}
				++id;
			}
		}
	}

	json["completed lore"] = completed_lore;

	auto current_act_index = std::distance(acts_.data(), current_act_);
	auto current_location_index = std::distance(current_act_->locations_.data(), current_location_);

	json["no stone current location"] = { current_act_index, current_location_index };
}

void NoStoneManager::LoadData(const nlohmann::json& save_json)
{
	std::ifstream file("assets/no-stone-unturned.json");
	if (!file.is_open()) {
		std::cout << "Error loading No Stone Unturned file";
		return;
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

				if (save_json.count("completed lore")) {
					for (const auto& item : save_json["completed lore"]) {
						if (id == item) {
							completed = true;
							break;
						}
					}
				}
				new_location.lore_.push_back(Lore{ lore, completed });
				++id;
			}

			if (location.count("note")) {
				new_location.note_ = location["note"];
			}
			new_act.locations_.push_back(new_location);
		}

		acts_.push_back(new_act);
	}

	if (save_json.count("no stone current location")) {
		auto indices = save_json["no stone current location"];
		current_act_ = &acts_[indices[0]];
		current_location_ = &current_act_->locations_[indices[1]];
	}
	else {
		current_act_ = &acts_[0];
		current_location_ = &current_act_->locations_[0];
	}
}
