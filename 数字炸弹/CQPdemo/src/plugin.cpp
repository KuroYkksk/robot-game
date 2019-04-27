#include <thread>

#include "plugin.h"

//循环负责分发所有的事件
void Plugin::runEventLoop()
{
	bool keepRunning = true;
	while (keepRunning) {
		std::unique_ptr<Event> eventPtr;
		if (m_eventChannel->get(eventPtr)) {
			switch (eventPtr->type) {
			case Event::Type::init:
				onInit();
				break;
			case Event::Type::exit:
				onExit();
				keepRunning = false;//退出
				break;
			case Event::Type::enabled:
				onEnabled();
				break;
			case Event::Type::disabled:
				onDisabled();
				break;
			case Event::Type::setAuthCode:
				m_authCode = reinterpret_cast<SetAuthCodeEvent*>(eventPtr.get())->authCode;
				break;
			case Event::Type::groupMessage:
				onGroupMessage(*reinterpret_cast<GroupMessageEvent*>(eventPtr.get()));
				break;
			}
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
}

Plugin::Plugin(Channel<std::unique_ptr<Event>>* channel)
	:m_eventChannel(channel)
{
}
