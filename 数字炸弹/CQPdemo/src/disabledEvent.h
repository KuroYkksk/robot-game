#pragma once
#include "event.h"

struct DisabledEvent :public Event {
	DisabledEvent()
		:Event(Type::disabled) {}
};