#pragma once
#include "event.h"

struct EnabledEvent:public Event {
	EnabledEvent()
		:Event(Type::enabled) {}
};