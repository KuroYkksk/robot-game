#pragma once

#include<cstdint>
#include<string>

#include "event.h"

struct GroupMessageEvent :public Event {

	GroupMessageEvent()
		:Event(Type::groupMessage) {};
	GroupMessageEvent(int32_t msgId, int64_t fromGroup, int64_t fromQQ, std::string msg)
		:Event(Type::groupMessage),msgId(msgId),fromGroup(fromGroup),fromQQ(fromQQ),msg(msg){}

	int32_t msgId;
	int64_t fromGroup;
	int64_t fromQQ;
	std::string msg;

};