#pragma once
#include "event.h"

class EnabledEvent:public Event {
	EnabledEvent()
		:Event(Type::enabled) {}
};