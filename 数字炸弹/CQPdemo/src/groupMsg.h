#pragma once
#include<cstdint>
#include<string>

struct GroupMsg
{
	int32_t msgId;
	int64_t fromGroup;
	int64_t fromQQ;
	std::string msg;

	GroupMsg()
	{}

	GroupMsg(int32_t msgId, int64_t fromGroup, int64_t fromQQ, std::string msg)
		:msgId(msgId), fromQQ(fromQQ), fromGroup(fromGroup), msg(msg) {}
};