#include "Step.h"

#include "imgui.h"
#include <string>
NpcStep::NpcStep(const std::string& npc, const std::vector<std::string>& dialog_list)
{
	npc_ = npc;

	int index = 0;
	for (auto& dialog : dialog_list) {
		dialogs_.push_back(Dialog({ dialog, "##" + std::to_string(index)}));
		++index;
	}

	display_text_ = "Talk to: " + npc_;
}

void NpcStep::Render()
{
	ImGui::TextWrapped(display_text_.c_str());

	for (auto& dialog : dialogs_) {
		ImGui::Checkbox(dialog.id.c_str(), &dialog.completed);
		ImGui::SameLine();
		ImGui::TextWrapped(dialog.name.c_str());
	}
	
}

TravelStep::TravelStep(const std::string& destination)
{
	destination_ = destination;
	display_text_ = "Travel to: " + destination;
}

void TravelStep::Render()
{
	ImGui::Text(display_text_.c_str());
}



EventStep::EventStep(const std::string& event)
{
	event_ = event;
}

void EventStep::Render()
{
	ImGui::Checkbox(event_.c_str(), &completed_);
}
