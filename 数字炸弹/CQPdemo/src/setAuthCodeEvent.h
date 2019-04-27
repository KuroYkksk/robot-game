#pragma once
#include "event.h"

struct SetAuthCodeEvent :public Event {
	SetAuthCodeEvent(int authCode)
		:Event(Type::setAuthCode),authCode(authCode) 
	{}
	int authCode;
};