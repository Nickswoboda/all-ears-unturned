#pragma once

#include <iostream>
#include <vector>

struct StepObjective
{
	std::string name;
	std::string id;
	bool completed = false;
};

class Step
{
public:
	virtual void Render() = 0;
	std::string note_;
};

class NpcStep : public Step
{
public:
	NpcStep(const std::string& npc, const std::vector<std::string>& dialogs);
	void Render() override;

	std::string npc_;
	std::vector<StepObjective> dialogs_;
	std::string display_text_;
};

class TravelStep : public Step
{
public:
	TravelStep(const std::string& destination);
	void Render() override;

	std::string destination_;
	std::string display_text_;
};

class EventStep : public Step
{
public:
	EventStep(const std::vector<std::string>& dialogs);
	void Render() override;

	std::vector<StepObjective> events_;
};