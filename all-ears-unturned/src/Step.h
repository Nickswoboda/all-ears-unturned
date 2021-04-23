#pragma once

#include <iostream>
#include <vector>

#include <json.hpp>

struct Objective
{
	std::string name_;
	std::string id_;
	bool completed_ = false;
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
	NpcStep(const nlohmann::json& json);
	void Render() override;

	std::string npc_;
	std::vector<Objective> dialogs_;
	std::string display_text_;
	int dialogs_completed_ = 0;
};

class TravelStep : public Step
{
public:
	TravelStep(const nlohmann::json& json);
	void Render() override;

	std::string destination_;
	std::string display_text_;
};

class EventStep : public Step
{
public:
	EventStep(const nlohmann::json& json);
	void Render() override;

	std::vector<Objective> events_;
};
