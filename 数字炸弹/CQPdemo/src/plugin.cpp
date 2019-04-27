#include "MsgSub.h"
#include "cqp.h"
#include <time.h>
#include <string>
using namespace std;

//导入全局变量 ac   在appmain.h 中定义
extern int ac;


int players = 0;			//玩家数
bool bombGame = false;		 //判断游戏是否开始的标志
int64_t bombPlayer[10] = { 0 };//参加游戏的玩家
int bombNumber = -1;			//炸弹数字，初始化为-1
int bombTurn = -1;			//猜数回合
int low = 1;		//初始化上下界
int high = 100;

//游戏开始
void bombStart(int64_t group)
{
	bombGame = true;
	srand((unsigned)time(NULL));
	bombNumber = rand() % (99 - 2 + 1) + 2;	//生成[2,99]的随机数
	low = 1;
	high = 100;
	bombTurn = 0;
	string bombHint = "【心跳~禁言数字炸弹】游戏开始\n数字已经生成~\n【1】——>【100】";
	for (int i = 0; i < players; i++)
	{
		bombHint += "[CQ:at,qq=" + to_string(bombPlayer[i]) + "]";
	}
	CQ_sendGroupMsg(ac, group, bombHint.c_str());
}
//游戏重置
void bombReset()
{
	bombGame = false;
	for (int i = 0; i < 10; i++)
	{
		bombPlayer[i] = 0;
	}
	bombNumber = -1;
	players = 0;
	bombTurn = -1;
	low = 1;
	high = 100;
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

//炸弹判断
bool bombJudge(int64_t group, string message)
{
	int judge;
	sscanf(message.c_str(), "**b %d", &judge);
	if (judge == bombNumber) {
		return true;
	}
	else if (low < judge && judge < bombNumber) {
		low = judge;
		return false;
	}else if (bombNumber < judge && judge < high) {
		high = judge;
		return false;
	}else {
		CQ_sendGroupMsg(ac, group, "【数字超出范围，请重新输入！】");
		bombTurn--;
		return false;
	}
}

void Plugin::threadMain()
{
	while (!m_quit)
	{
		GroupMsg msg;
		if (m_channel->get(msg)){

			string bombHint;			//发送游戏提示
			bool bombInclude = false;	//检查玩家是否已报名
			//当第一个字符是*时读取指令
			if (msg.msg.substr(0, 1) == "*")	
			{
				if (msg.msg == "*bomb join")
				{
					//当游戏未开始时
					if (bombGame == false) {

						//检查玩家是否已报名
						for (int i = 0; i < 10; i++)
						{
							if (bombPlayer[i] == msg.fromQQ)
							{
								bombInclude = true;
								break;
							}
							bombInclude = false;
						}

						if (bombInclude)
						{
							CQ_sendGroupMsg(ac, msg.fromGroup, "【报名失败，你已报名】");
						}
						else if (players >= 10)
						{
							CQ_sendGroupMsg(ac, msg.fromGroup, "【报名失败，玩家数已满】");
						}
						else
						{
							players++;
							bombHint = "【报名成功，当前有" + to_string(players) + "位玩家】";
							CQ_sendGroupMsg(ac, msg.fromGroup, bombHint.c_str());
							bombPlayer[players - 1] = msg.fromQQ;	//将发送信息的QQ存入数组
						}
					}
					else {
						CQ_sendGroupMsg(ac, msg.fromGroup, "【报名失败，游戏已经开始】");
					}
				}
				else if (msg.msg == "*bomb quit")
				{
					int bombIndex = -1;
					//检查玩家是否已报名
					for (int i = 0; i < 10; i++)
					{
						if (bombPlayer[i] == msg.fromQQ)
						{
							bombInclude = true;
							int bombIndex = -1;
							break;
						}
						bombInclude = false;
					}

					if (bombGame == true)
					{
						CQ_sendGroupMsg(ac, msg.fromGroup, "【取消报名失败，游戏已经开始】");
					}
					else if (bombInclude)
					{
						//将被取消报名的玩家后面的每个玩家向前移动一位
						for (int i = bombIndex; i < 10; i++)
						{
							bombPlayer[i] = i == 9 ? 0 : bombPlayer[i + 1];
						}
						players--;
						bombHint = "【取消报名成功，当前有" + to_string(players) + "名玩家】";
						CQ_sendGroupMsg(ac, msg.fromGroup, bombHint.c_str());
					}
					else {
						CQ_sendGroupMsg(ac, msg.fromGroup, "【取消报名失败，你没有报名】");
					}
				}

				else if (msg.msg == "*bomb start")
				{
					if (bombGame == false) {
						if (players < 1)
						{
							CQ_sendGroupMsg(ac, msg.fromGroup, "【开始失败，玩家不足】");
						}
						else {
							Disorder(bombPlayer, players);
							bombStart(msg.fromGroup);
							bombHint = "【首先由[CQ:at,qq=" + to_string(bombPlayer[bombTurn]) + "]猜数】";
							CQ_sendGroupMsg(ac, msg.fromGroup, bombHint.c_str());
						}
					}
					else {
						CQ_sendGroupMsg(ac, msg.fromGroup, "【开始失败，游戏已经开始】");
					}
				}
				else if (msg.msg == "*bomb reset")
				{
					bombReset();
					CQ_sendGroupMsg(ac, msg.fromGroup, "【数字炸弹重置完成】");
				}
				else if (msg.msg.substr(0, 3) == "**b")
				{
					if (bombGame == true) {
						if (bombPlayer[bombTurn] == msg.fromQQ)
						{
							if (bombJudge(msg.fromGroup, msg.msg))
							{
								bombHint = "【嘭————————】\n[CQ:at,qq=" + to_string(bombPlayer[bombTurn]) + "]\n被禁言1分钟\n游戏结束";
								CQ_setGroupBan(ac, msg.fromGroup, msg.fromQQ, 60);//禁言60s
								CQ_sendGroupMsg(ac, msg.fromGroup, bombHint.c_str());
								bombReset();
							}
							else
							{
								bombTurn++;
								//玩家循环
								if (bombTurn == players) {
									bombTurn = 0;
								}
								bombHint = "【" + to_string(low) + "】——>【" + to_string(high) + "】\n";
								bombHint += "【轮到[CQ:at,qq=" + to_string(bombPlayer[bombTurn]) + "]猜数】";
								CQ_sendGroupMsg(ac, msg.fromGroup, bombHint.c_str());
							}
						}
						else {
							CQ_sendGroupMsg(ac, msg.fromGroup, "【你未参加游戏，或尚未轮到你的回合】");
						}
					}
					else if(bombGame == false)
					{
						CQ_sendGroupMsg(ac, msg.fromGroup, "【游戏尚未开始】");
					}
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

Plugin::Plugin(Channel<GroupMsg>* channel)
{
	m_quit = false;
	m_channel = channel;
}

Plugin::~Plugin()
{
}

void Plugin::start()
{
	m_quit = false;
	ThreadBase::start();
}

void Plugin::quite()
{
	ThreadBase::quit();
	m_quit = true;
}



