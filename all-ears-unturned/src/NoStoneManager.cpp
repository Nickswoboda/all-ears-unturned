#include "NoStoneManager.h"

#include <imgui.h>
#include <string>
NoStoneManager::NoStoneManager()
{
	for (int i = 0; i < 20; ++i) {
		std::string temp = std::to_string(i);
		list_.push_back(temp);
	}
}
void NoStoneManager::Render()
{
	static int selected = 0;
	ImGui::Combo("##Act", &selected, "Act1\0Act2\0Act3");
	ImGui::SameLine();
	ImGui::Combo("##Area", &selected, "Lioneyes\0Coast\0Tidal Island");
	ImGui::Separator();

	static bool bool_ = false;
	static int page_ = 1;
	int num_pages = (list_.size() / max_items) + 1;
	for (int i = (page_ - 1) * max_items; i < max_items * page_ && i < list_.size(); ++i) {
		
		ImGui::Checkbox(list_[i].c_str(), &bool_);
		++i;

		if (i < max_items * page_ && i < list_.size()) {
			ImGui::SameLine(333);
			ImGui::Checkbox(list_[i].c_str(), &bool_);
		}
	}

	ImGui::Text("Page %d / %d", page_, num_pages);
	if (ImGui::Button("Next Page")) {
		++page_;
	}
}
