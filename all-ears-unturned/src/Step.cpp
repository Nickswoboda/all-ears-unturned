#include "Step.h"

#include "imgui.h"
NpcStep::NpcStep(const std::string& npc, const std::vector<std::string>& dialog_list)
{
	npc_ = npc;

	for (auto& dialog : dialog_list) {
		dialogs_.push_back(Dialog({ dialog }));
	}
}

void NpcStep::Render()
{
	ImGui::Text("Talk to: ");
	ImGui::SameLine(70);
	ImGui::Text(npc_.c_str());
	for (auto& dialog : dialogs_) {
		ImGui::Checkbox(dialog.name.c_str(), &dialog.completed);
	}
}

TravelStep::TravelStep(const std::string& destination)
{
	destination_ = destination;
}

void TravelStep::Render()
{
	ImGui::Text("Travel to: ");
	ImGui::SameLine(85);
	ImGui::Text(destination_.c_str());
}



EventStep::EventStep(const std::string& event)
{
	event_ = event;
}

void EventStep::Render()
{
	ImGui::Checkbox(event_.c_str(), &completed_);
}
