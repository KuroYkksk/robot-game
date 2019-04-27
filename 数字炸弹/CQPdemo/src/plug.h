#pragma once

#include<memory>

#include "channel.h"
#include "events.h"

class NPlugin {
public:
	NPlugin(Channel<std::unique_ptr<Event>>*);
	virtual ~NPlugin() = 0;

	//���߳����е�ѭ��
	void runEventLoop();
protected:
	//�¼���������Ĭ��ʲô������
	virtual void onInit() {};
	virtual void onExit() {};
	virtual void onEnabled() {};
	virtual void onDisabled() {};
	virtual void onGroupMessage(const GroupMessageEvent& event) {};
private:
	int m_authCode;
	Channel<std::unique_ptr<Event>>* m_eventChannel;
};