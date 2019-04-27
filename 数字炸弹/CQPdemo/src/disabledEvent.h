#pragma once
#include "event.h"

class DisabledEvent :public Event {
	DisabledEvent()
		:Event(Type::disabled) {}
};