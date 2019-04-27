#include <thread>

#include "plug.h"

//ѭ������ַ����е��¼�
void NPlugin::runEventLoop()
{
	bool keepRunning = true;
	while (keepRunning) {
		std::unique_ptr<Event> eventPtr;
		if (m_eventChannel->get(eventPtr)) {
			switch (eventPtr->type) {
			case Event::Type::startup:
				onInit();
				break;
			case Event::Type::shutdown:
				onExit();
				keepRunning = false;//�˳�
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

NPlugin::NPlugin(Channel<std::unique_ptr<Event>>* channel)
	:m_eventChannel(channel)
{
}
