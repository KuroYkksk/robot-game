#pragma once

#include<memory>

#include "channel.h"
#include "events.h"

class NPlugin {
public:
	NPlugin(Channel<std::unique_ptr<Event>>*);
	virtual ~NPlugin() = 0;

	//让线程运行的循环
	void runEventLoop();
protected:
	//事件处理函数，默认什么都不做
	virtual void onInit() {};
	virtual void onExit() {};
	virtual void onEnabled() {};
	virtual void onDisabled() {};
	virtual void onGroupMessage(const GroupMessageEvent& event) {};
private:
	int m_authCode;
	Channel<std::unique_ptr<Event>>* m_eventChannel;
};