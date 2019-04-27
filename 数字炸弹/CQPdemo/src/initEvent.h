#pragma once
#include "event.h"
struct InitEvent :public Event {
	InitEvent() :Event(Type::init) {}
};