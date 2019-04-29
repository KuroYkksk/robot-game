#include "MsgSub.h"
#include "cqp.h"
#include <time.h>
#include <string>
using namespace std;

//导入全局变量 ac   在appmain.h 中定义
extern int ac;


int players = 0;			//玩家数
bool ruroGame = false;		 //判断游戏是否开始的标志
int64_t ruroPlayer[6] = { 0 };//参加轮盘的玩家
int bullet = -1;			//子弹的位置，初始化为-1
int shootTurn = -1;			//开枪回合

//游戏开始
void ruroStart(int64_t group)
{
	ruroGame = true;
	srand((unsigned)time(NULL));
	bullet = rand() % 6;	//生成[1,6]的随机数
	shootTurn = 0;
	string ruroHint = "【心跳~禁言俄罗斯轮盘】游戏开始\n1发子弹已装填~\n";
	for (int i = 0; i < players; i++)
	{
		ruroHint += "[CQ:at,qq=" + to_string(ruroPlayer[i]) + "]";
	}
	CQ_sendGroupMsg(ac, group, ruroHint.c_str());
}
//游戏重置
void ruroReset()
{
	ruroGame = false;
	for (int i = 0; i < 6; i++)
	{
		ruroPlayer[i] = 0;
	}
	bullet = -1;
	players = 0;
	shootTurn = -1;
}

//玩家乱序算法
void Disorder(int64_t Player[], int player)
{
	int index, i;
	int64_t tmp;
	srand((unsigned)time(NULL));
	for (i = 0; i < player; i++)
	{
		index = rand() % (player - i) + i;
		tmp = Player[i];
		Player[i] = Player[index];
		Player[index] = tmp;
	}
}

void GroupMsgSub::threadMain()
{
	while (!m_quit)
	{
		if (m_groupMsgBuffer.size() > 0)
		{
			m_mutex.lock();
			GroupMsg msg = m_groupMsgBuffer.front();
			m_groupMsgBuffer.pop();
			m_mutex.unlock();

			string ruroHint;			//发送游戏提示
			bool ruroInclude = false;	//检查玩家是否已报名
			//当第一个字符是*时读取指令
			if (msg.msg.substr(0, 5) == "*ruro")	
			{
				if (msg.msg == "*ruro join")
				{
					//当游戏未开始时
					if (ruroGame == false) {

						//检查玩家是否已报名
						for (int i = 0; i < 6; i++)
						{
							if (ruroPlayer[i] == msg.fromQQ)
							{
								ruroInclude = true;
								break;
							}
							ruroInclude = false;
						}

						if (ruroInclude)
						{
							CQ_sendGroupMsg(ac, msg.fromGroup, "【报名失败，你已报名】");
						}
						else if (players >= 6)
						{
							CQ_sendGroupMsg(ac, msg.fromGroup, "【报名失败，玩家数已满】");
						}
						else
						{
							players++;
							ruroHint = "【报名成功，当前有" + to_string(players) + "位玩家】";
							CQ_sendGroupMsg(ac, msg.fromGroup, ruroHint.c_str());
							ruroPlayer[players - 1] = msg.fromQQ;	//将发送信息的QQ存入数组
						}
					}
					else {
						CQ_sendGroupMsg(ac, msg.fromGroup, "【报名失败，游戏已经开始】");
					}
				}
				else if (msg.msg == "*ruro quit")
				{
					int ruroIndex = -1;
					//检查玩家是否已报名
					for (int i = 0; i < 6; i++)
					{
						if (ruroPlayer[i] == msg.fromQQ)
						{
							ruroInclude = true;
							ruroIndex = i;
							break;
						}
						ruroInclude = false;
					}

					if (ruroGame == true)
					{
						CQ_sendGroupMsg(ac, msg.fromGroup, "【取消报名失败，游戏已经开始】");
					}
					else if (ruroInclude)
					{
						//将被取消报名的玩家后面的每个玩家向前移动一位
						for (int i = ruroIndex; i < 6; i++)
						{
							ruroPlayer[i] = i == 5 ? 0 : ruroPlayer[i + 1];
						}
						players--;
						ruroHint = "【取消报名成功，当前有" + to_string(players) + "名玩家】";
						CQ_sendGroupMsg(ac, msg.fromGroup, ruroHint.c_str());
					}
					else {
						CQ_sendGroupMsg(ac, msg.fromGroup, "【取消报名失败，你没有报名】");
					}
				}

				else if (msg.msg == "*ruro start")
				{
					if (ruroGame == false) {

						//检查玩家是否已报名
						for (int i = 0; i < 10; i++)
						{
							if (ruroPlayer[i] == msg.fromQQ)
							{
								ruroInclude = true;
								break;
							}
							ruroInclude = false;
						}

						if (players < 1)
						{
							CQ_sendGroupMsg(ac, msg.fromGroup, "【开始失败，玩家不足】");
						}
						else if (ruroInclude) {
							Disorder(ruroPlayer, players);
							ruroStart(msg.fromGroup);
							ruroHint = "【首先由[CQ:at,qq=" + to_string(ruroPlayer[shootTurn]) + "]开枪】";
							CQ_sendGroupMsg(ac, msg.fromGroup, ruroHint.c_str());
						}
						else {
							CQ_sendGroupMsg(ac, msg.fromGroup, "【你没有报名，请不要迫害玩家~】");
						}

					}
					else {
						CQ_sendGroupMsg(ac, msg.fromGroup, "【开始失败，游戏已经开始】");
					}
				}
				else if (msg.msg == "*ruro reset")
				{
					ruroReset();
					CQ_sendGroupMsg(ac, msg.fromGroup, "【俄罗斯轮盘重置完成】");
				}
			}
			if (msg.msg == "**shoot")
			{
				if (ruroGame == true) {
					if (ruroPlayer[shootTurn] == msg.fromQQ)
					{
						if (msg.msg == "**shoot")
						{
							bullet--;
							shootTurn++;
							if (bullet == 0) {
								ruroHint = "【砰————————】\n[CQ:at,qq=" + to_string(ruroPlayer[shootTurn-1]) + "]\n被禁言1分钟\n游戏结束";
								CQ_setGroupBan(ac, msg.fromGroup, msg.fromQQ, 60);//禁言60s
								CQ_sendGroupMsg(ac, msg.fromGroup, ruroHint.c_str());
								ruroReset();
							}
							else if (shootTurn == players) {
								CQ_sendGroupMsg(ac, msg.fromGroup, "【所有玩家存活~~游戏结束】");
								ruroReset();
							}
							else if (shootTurn != players) {
								CQ_sendGroupMsg(ac, msg.fromGroup, "【什么都没发生~】");
								ruroHint = "【轮到[CQ:at,qq=" + to_string(ruroPlayer[shootTurn]) + "]开枪】";
								CQ_sendGroupMsg(ac, msg.fromGroup, ruroHint.c_str());
							}
						}
					}
					else {
						CQ_sendGroupMsg(ac, msg.fromGroup, "【你未参加游戏，或尚未轮到你的回合】");
					}
				}
				else {
					CQ_sendGroupMsg(ac, msg.fromGroup, "【游戏尚未开始】");
				}
			}



		}
		else
		{
			ThreadBase::mSleep(10);
		}
	}
	return;
}

GroupMsgSub::GroupMsgSub()
{
	m_quit = false;
}

GroupMsgSub::~GroupMsgSub()
{
}

void GroupMsgSub::start()
{
	m_quit = false;
	ThreadBase::start();
}

void GroupMsgSub::quite()
{
	ThreadBase::quit();
	m_quit = true;
}


void GroupMsgSub::pushMsg(int32_t msgId, int64_t fromGroup, int64_t fromQQ, std::string msg)
{
	//如果缓存的消息大于100跳  则直接抛弃新到的消息
	if (m_groupMsgBuffer.size() < 100)
	{
		m_mutex.lock();
		m_groupMsgBuffer.push(GroupMsg(msgId, fromGroup, fromQQ, msg));
		m_mutex.unlock();
	}
}
