#include "Step.h"

#include "imgui.h"
#include <string>
NpcStep::NpcStep(const nlohmann::json& json)
{
	npc_ = json["NPC"];

	int index = 0;
	for (auto& dialog : json["subjects"]) {
		dialogs_.push_back(Objective({ dialog, "##" + std::to_string(index)}));
		++index;
	}

	display_text_ = "Talk to: " + npc_;

	if (json.count("note")) {
		note_ = json["note"];
	}
}

void NpcStep::Render()
{
	if (!note_.empty()) {
		ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 0.0f, 0.0f, 1.0f });
		ImGui::TextWrapped(note_.c_str());
		ImGui::PopStyleColor();
	}

	ImGui::TextWrapped(display_text_.c_str());

	for (auto& dialog : dialogs_) {
		if (ImGui::Checkbox(dialog.id_.c_str(), &dialog.completed_)){
			if (dialog.completed_){
				++dialogs_completed_;
			} else {
				--dialogs_completed_;
			}
		}
		ImGui::SameLine();
		ImGui::TextWrapped(dialog.name_.c_str());
	}
}

TravelStep::TravelStep(const nlohmann::json& json)
{
	destination_ = json["destination"];
	display_text_ = "Travel to: " + destination_;

	if (json.count("note")) {
		note_ = json["note"];
	}
}

void TravelStep::Render()
{
	ImGui::TextWrapped(display_text_.c_str());
	if (!note_.empty()) {
		ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 0.0f, 0.0f, 1.0f });
		ImGui::TextWrapped(note_.c_str());
		ImGui::PopStyleColor();
	}
}



EventStep::EventStep(const nlohmann::json& json)
{
	int index = 0;
	for (auto& event : json["event"]) {
		events_.push_back(Objective({ event, "##" + std::to_string(index) }));
		++index;
	}

	if (json.count("note")) {
		note_ = json["note"];
	}
}

void EventStep::Render()
{
	for (auto& event : events_) {
		ImGui::Checkbox(event.id_.c_str(), &event.completed_);
		ImGui::SameLine();
		ImGui::TextWrapped(event.name_.c_str());
	}

	if (!note_.empty()) {
		ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 0.0f, 0.0f, 1.0f });
		ImGui::TextWrapped(note_.c_str());
		ImGui::PopStyleColor();
	}
}
