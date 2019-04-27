#pragma once
#include "event.h"

class SetAuthCodeEvent :public Event {
	SetAuthCodeEvent(int authCode)
		:Event(Type::setAuthCode),authCode(authCode) 
	{}
	int authCode;
};