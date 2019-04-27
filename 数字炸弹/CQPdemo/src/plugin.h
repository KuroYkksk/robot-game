#pragma once
#include <string>
#include <queue>
#include "ThreadBase.h"
#include "channel.h"
#include <map>

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

class Plugin :
	public ThreadBase
{
public:
	Plugin(Channel<GroupMsg>* channel);
	~Plugin();

	void start();
	void quite();
	void pushMsg(int32_t msgId, int64_t fromGroup, int64_t fromQQ, std::string msg);
private:
	virtual void threadMain();
private:
	bool m_quit;   //是否退出
	Channel<GroupMsg>* m_channel;//获取信息的通道
};



