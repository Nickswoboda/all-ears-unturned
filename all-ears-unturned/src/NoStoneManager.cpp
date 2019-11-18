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
	if (ImGui::BeginCombo("Act", acts_[current_act_].name_.c_str())) {
		for (int i = 0; i < acts_.size(); ++i) {
			if (ImGui::Selectable(acts_[i].name_.c_str(), current_act_ == i))
				current_act_ = i;
		}

		//selectable popup does not close if user clicks out of window and loses focus
		//must do manually
		if (!Window::IsFocused()) {
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndCombo();
	}

	if (ImGui::BeginCombo("Location", acts_[current_act_].locations_[current_location_].c_str())) {
		if (Window::IsFocused()) {
			for (int i = 0; i < acts_[current_act_].locations_.size(); ++i) {
				if (ImGui::Selectable(acts_[current_act_].locations_[i].c_str(), current_location_ == i))
					current_location_ = i;
			}
		}

		if (!Window::IsFocused()) {
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndCombo();
	}

	ImGui::Separator();

	static bool bool_ = false;
	static int page_ = 1;
	int num_items = acts_[current_act_].lore_[current_location_].size();
	int num_pages = (num_items / 7) + 1;
	for (int i = (page_ - 1) * 7; i < 7 * page_ && i < num_items; ++i) {
		
		ImGui::Checkbox(acts_[current_act_].lore_[current_location_][i].name.c_str(), &acts_[current_act_].lore_[current_location_][i].completed);
		++i;
	
		if (i < 7 * page_ && i < num_items) {
			ImGui::SameLine(333);
			ImGui::Checkbox(acts_[current_act_].lore_[current_location_][i].name.c_str(), &acts_[current_act_].lore_[current_location_][i].completed);
		}
	}

	ImGui::Text("Page %d / %d", page_, num_pages);
	if (ImGui::Button("Next Page")) {
		++page_;
	}
}

void NoStoneManager::ChangeLocation(const std::string& location)
{
	for (int i = 0; i < acts_.size(); ++i) {
		for (int j = 0; j < acts_[i].locations_.size(); ++j) {
			if (acts_[i].locations_[j] == location) {
				current_act_ = i;
				current_location_ = j;
				return;
			}
		}
	}
}

void NoStoneManager::LoadData(std::vector<int> completed_lore)
{
	std::ifstream file("assets/no-stone-unturned.json");
	if (!file.is_open()) {
		std::cout << "Error loading No Stone Unturned file";
	}
	nlohmann::json json = nlohmann::json::parse(file);

	file.close();

	int id = 0;
	for (const auto& act : json.items()) {

		auto new_act = ActLore();
		new_act.name_ = act.key();

		for (const auto& location : act.value()) {
			std::string key = location.begin().key();
			new_act.locations_.push_back(key);
			std::vector<Lore> lore_temp;
			for (const auto& lore : location[key]) {
				bool completed = false;

				for (const auto& item : completed_lore) {
					if (id == item) {
						completed = true;
						break;
					}
				}
				lore_temp.push_back(Lore{ lore, completed });
				++id;
			}
			new_act.lore_.push_back(lore_temp);
		}

		acts_.push_back(new_act);
	}
}
