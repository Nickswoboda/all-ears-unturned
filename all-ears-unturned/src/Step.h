#pragma once

#include <iostream>
#include <vector>
class Step
{
public:
	virtual void Render() = 0;
};

class NpcStep : public Step
{
public:
	struct Dialog
	{
		std::string name;
		bool completed = false;
	};

	NpcStep(const std::string& npc, const std::vector<std::string>& dialogs);
	void Render() override;

private:
	std::string npc_;
	std::vector<Dialog> dialogs_;
};

class TravelStep : public Step
{
public:
	TravelStep(const std::string& destination);
	void Render() override;

private:
	std::string destination_;
};

class EventStep : public Step
{
public:
	EventStep(const std::string& event);
	void Render() override;

private:
	std::string event_;
	bool completed_ = false;
};