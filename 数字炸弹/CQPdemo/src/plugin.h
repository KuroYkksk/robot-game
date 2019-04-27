#pragma once

#include<memory>

#include "channel.h"
#include "events.h"

class Plugin {
public:
	Plugin(Channel<std::unique_ptr<Event>>*);
	virtual ~Plugin() {};

	//���߳����е�ѭ��
	void runEventLoop();
protected:

	int32_t authCode() {
		return m_authCode;
	}

	//�¼���������Ĭ��ʲô������
	virtual void onInit() {};
	virtual void onExit() {};
	virtual void onEnabled() {};
	virtual void onDisabled() {};
	virtual void onGroupMessage(const GroupMessageEvent& event) {};
private:
	int32_t m_authCode;
	Channel<std::unique_ptr<Event>>* m_eventChannel;
};