#pragma once

#include<memory>

#include "channel.h"
#include "events.h"

class Plugin {
public:
	Plugin(Channel<std::unique_ptr<Event>>*);
	virtual ~Plugin() {};

	//让线程运行的循环
	void runEventLoop();
protected:

	int32_t authCode() {
		return m_authCode;
	}

	//事件处理函数，默认什么都不做
	virtual void onInit() {};
	virtual void onExit() {};
	virtual void onEnabled() {};
	virtual void onDisabled() {};
	virtual void onGroupMessage(const GroupMessageEvent& event) {};
private:
	int32_t m_authCode;
	Channel<std::unique_ptr<Event>>* m_eventChannel;
};