#pragma once
#include "event.h"
struct ExitEvent:public Event {
	ExitEvent() :Event(Type::exit) {}
};