#pragma once
#include <string>
#include <queue>
#include <map>

#include "plugin.h"

class BombGamePlugin:public Plugin
{
public:
	BombGamePlugin(Channel<std::unique_ptr<Event>>* ptr);
	virtual ~BombGamePlugin() = default;
protected:
	void bombStart(int64_t group);
	bool bombJudge(int64_t group, std::string message);
	virtual void onGroupMessage(const GroupMessageEvent& event) override;
};



