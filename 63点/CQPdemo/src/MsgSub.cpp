#include "MsgSub.h"
#include "cqp.h"
#include <time.h>
#include <string>
#include <vector>
using namespace std;

//导入全局变量 ac   在appmain.h 中定义
extern int ac;

//63点游戏，游戏名将缩写为ST
int players = 0;			//玩家数
bool STGame = false;		 //判断游戏是否开始的标志
int64_t STPlayer[4] = { 0 };//参加63点的玩家
int STTurn = -1;			//抽卡回合
string pokerCardName[4][13] = { 0 };//给每一张卡命名
int pokerCard[52] = { -1 };	//创建一副去鬼牌的扑克牌
vector<int> playerCard;		//发给玩家的牌
int virtualTotal = 0;		//显示的合计值
int realTotal = 0;			//实际的合计值
int call = 0;				//宣言值
int realtmp = 0;			//存放实际点数


//游戏重置
void STReset()
{
	STGame = false;
	for (int i = 0; i < 4; i++)
	{
		STPlayer[i] = 0;
	}
	players = 0;
	STTurn = -1;
	pokerCardName[4][13] = { 0 };
	pokerCard[52] = { -1 };
	playerCard.clear();
	virtualTotal = 0;
	realTotal = 0;
	call = 0;
	realtmp = 0;
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
		if (index != i) {
			tmp = Player[i];
			Player[i] = Player[index];
			Player[index] = tmp;
		}
	}
}
//洗牌
void cardDisorder(int card[], int nId)
{
	int index, i, tmp;
	srand((unsigned)time(NULL));
	for (i = 0; i < nId; i++)
	{
		index = rand() % (nId - i) + i;
		if (index != i) {
			tmp = card[i];
			card[i] = card[index];
			card[index] = tmp;
		}
	}
}
//生成牌组
void pokerNameSet()
{
	string pokerNameStr;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 13; j++)
		{
			if (i == 0) { pokerNameStr = "【黑桃 "; }
			else if (i == 1) { pokerNameStr = "【红桃 "; }
			else if (i == 2) { pokerNameStr = "【梅花 "; }
			else { pokerNameStr = "【方块 "; }

			if (j == 0) { pokerNameStr += "A】"; }
			else if (0 < j && j < 10) { pokerNameStr += to_string(j + 1) + "】"; }
			else if (j == 10) { pokerNameStr += "J】"; }
			else if (j == 11) { pokerNameStr += "Q】"; }
			else { pokerNameStr += "K】"; }

			pokerCardName[i][j] = pokerNameStr;
		}
	}
}
//对每一张牌命名
string pokerName(int id)
{
	string name;
	name = pokerCardName[id / 13][id % 13];
	return name;
}
//获取牌的实际点数
int point(int id)
{
	int tmp = id % 13;
	if (tmp < 10) { return tmp + 1; }
	else { return 10; }
}
//游戏开始
void STStart(int64_t group)
{
	STGame = true;
	STTurn = 0;
	virtualTotal = 0;
	realTotal = 0;
	call = 0;
	realtmp = 0;

	//生成牌堆
	pokerNameSet();
	for (int i = 0; i < 52; i++)
	{
		pokerCard[i] = i;
	}
	cardDisorder(pokerCard, 52);
	for (int i = 0; i < 52; i++)
	{
		playerCard.push_back(i);
	}

	string STHint = "【63点】游戏开始\n牌组设置完毕~\n";
	for (int i = 0; i < players; i++)
	{
		STHint += "[CQ:at,qq=" + to_string(STPlayer[i]) + "]";
	}
	CQ_sendGroupMsg(ac, group, STHint.c_str());
}
//游戏结果判断
string STEndJudge(string hint)
{
	hint += "显示合计值【" + to_string(virtualTotal) + "/63】\n";
	hint += "实际合计值【" + to_string(realTotal) + "/63】\n";
	if (realTotal <= 63)
	{
		hint += "【玩家[CQ:at,qq=" + to_string(STPlayer[0]) + "]获胜~】";
	}
	else {	
		hint += "【玩家";
		for (int i = 0; i < players; i++)
		{
			hint += "[CQ:at,qq=" + to_string(STPlayer[i]) + "]";
		}
		hint += "获胜~】";
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

			string STHint;			//发送游戏提示
			bool STInclude = false;	//检查玩家是否已报名
			//当第一个字符是*时读取指令
			if (msg.msg.substr(0, 3) == "*63")	
			{
				if (msg.msg == "*63 join")
				{
					//当游戏未开始时
					if (STGame == false) {

						//检查玩家是否已报名
						for (int i = 0; i < 4; i++)
						{
							if (STPlayer[i] == msg.fromQQ)
							{
								STInclude = true;
								break;
							}
							STInclude = false;
						}

						if (STInclude)
						{
							CQ_sendGroupMsg(ac, msg.fromGroup, "【报名失败，你已报名】");
						}
						else if (players >= 4)
						{
							CQ_sendGroupMsg(ac, msg.fromGroup, "【报名失败，玩家数已满】");
						}
						else
						{
							players++;
							STHint = "【报名成功，当前有" + to_string(players) + "位玩家】";
							CQ_sendGroupMsg(ac, msg.fromGroup, STHint.c_str());
							STPlayer[players - 1] = msg.fromQQ;	//将发送信息的QQ存入数组
						}
					}
					else {
						CQ_sendGroupMsg(ac, msg.fromGroup, "【报名失败，游戏已经开始】");
					}
				}
				else if (msg.msg == "*63 quit")
				{
					int STIndex = -1;
					//检查玩家是否已报名
					for (int i = 0; i < 4; i++)
					{
						if (STPlayer[i] == msg.fromQQ)
						{
							STInclude = true;
							STIndex = i;
							break;
						}
						STInclude = false;
					}

					if (STGame == true)
					{
						CQ_sendGroupMsg(ac, msg.fromGroup, "【取消报名失败，游戏已经开始】");
					}
					else if (STInclude)
					{
						//将被取消报名的玩家后面的每个玩家向前移动一位
						for (int i = STIndex; i < 4; i++)
						{
							STPlayer[i] = i == 3 ? 0 : STPlayer[i + 1];
						}
						players--;
						STHint = "【取消报名成功，当前有" + to_string(players) + "名玩家】";
						CQ_sendGroupMsg(ac, msg.fromGroup, STHint.c_str());
					}
					else {
						CQ_sendGroupMsg(ac, msg.fromGroup, "【取消报名失败，你没有报名】");
					}
				}

				else if (msg.msg == "*63 start")
				{
					if (STGame == false) {

						//检查玩家是否已报名
						for (int i = 0; i < 4; i++)
						{
							if (STPlayer[i] == msg.fromQQ)
							{
								STInclude = true;
								break;
							}
							STInclude = false;
						}

						if (players < 1)
						{
							CQ_sendGroupMsg(ac, msg.fromGroup, "【开始失败，玩家不足】");
						}
						else if (STInclude) {
							Disorder(STPlayer, players);
							STStart(msg.fromGroup);
							STHint = "【首先由[CQ:at,qq=" + to_string(STPlayer[STTurn]) + "]开枪】";
							CQ_sendGroupMsg(ac, msg.fromGroup, STHint.c_str());
						}
						else {
							CQ_sendGroupMsg(ac, msg.fromGroup, "【你没有报名，请不要迫害玩家~】");
						}
					}
					else {
						CQ_sendGroupMsg(ac, msg.fromGroup, "【开始失败，游戏已经开始】");
					}
				}
				else if (msg.msg == "*63 reset")
				{
					STReset();
					CQ_sendGroupMsg(ac, msg.fromGroup, "【俄罗斯轮盘重置完成】");
				}
			}

			//重整
			if (msg.msg.substr(0, 5) == "**set")
			{
				if (STGame == true) {
					if (STPlayer[STTurn] == msg.fromQQ)
					{
						STTurn++;
						sscanf(msg.msg.c_str(), "**set %d", &call);
						if (call < 1 && call > 10) {
							STHint = "【数字不符合规则，请重新宣言】";
							CQ_sendGroupMsg(ac, msg.fromGroup, STHint.c_str());
						}
						else{
							virtualTotal += call;
							realTotal += point(realtmp);
							STHint = "【玩家宣言：" + to_string(call) + "点】\n当前合计值【" + to_string(virtualTotal) + "/63】";
							CQ_sendGroupMsg(ac, msg.fromGroup, STHint.c_str());

							if (playerCard.empty())
							{
								STHint = "【牌组抽空，游戏结束】\n";
								STEndJudge(STHint);
							}
							else {
								if (STTurn == players) { STTurn = 0; } //玩家循环
								STHint = "【[CQ:at,qq=" + to_string(STPlayer[STTurn]) + "]抽了一张牌】";
								CQ_sendGroupMsg(ac, msg.fromGroup, STHint.c_str());
								//私聊发牌
								realtmp = playerCard.back();
								STHint = "你抽到了一张：" + pokerName(realtmp) + "\n请在群里发送(**set)【宣言/放置】或(**pass)【放弃】做出决定";
								CQ_sendPrivateMsg(ac, STPlayer[STTurn], STHint.c_str());
								playerCard.pop_back();
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

			if (msg.msg.substr(0, 6) == "**pass")
			{
				if (STGame == true) {
					if (STPlayer[STTurn] == msg.fromQQ)
					{
						//除去玩家
						for (int i = STTurn; i < 6; i++)
						{
							STPlayer[i] = i == 5 ? 0 : STPlayer[i + 1];
						}
						players--;

						if (players == 1)
						{
							STHint = "【玩家放弃，游戏结束】\n";
							STEndJudge(STHint);
						}
						else {
							STHint = "【玩家放弃，剩余" + to_string(players) + "名玩家】";
							CQ_sendGroupMsg(ac, msg.fromGroup, STHint.c_str());
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
			if (msg.msg == "*63 rule")
			{
				CQ_sendGroupMsg(ac, msg.fromGroup, "【63点游戏规则】\n「63点」是使场上的共计数极限向「63点」前进的胆量对决,\n使用的是扑克牌52张（小丑牌不用），数字牌保持原状，花牌则算「10」来记数，\n每位玩家先从牌组抽一张牌，抽出的牌只能自己看，再将牌的数值「宣言」，以牌背为上「放置」到场上，\n到时候各位玩家可以讲真话，也可以说假话，\n依照这样的操作，按顺序进行，场上的共计数就会不断上升，还是混入了虚与实的共计数，\n由于这场游戏是胆量对决，所以共计数不能超过63。要是在自己的回合，觉得要到63了，可以选择「放弃」，\n选择「放弃」的人，当场游戏结束，像这样一人一人的退出，只留下最后一人的时候就会摊牌，\n场上真•共计数在「63」以下的话，胆量对决成功，最后留下的人会获胜，\n场上的真•共计数在「63」以上的话，胆量对决失败，最后留下的人会输掉。");
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
