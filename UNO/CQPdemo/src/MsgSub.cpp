#include "MsgSub.h"
#include "cqp.h"
#include <time.h>
#include <string>
#include <vector>
#include <set>

using namespace std;

//导入全局变量 ac   在appmain.h 中定义
extern int ac;

bool unoGame = false;
int64_t unoPlayer[10] = { 0 };
int unoNum = 0;
int unoTurn = -1;
int unoCardTurn = 0;
string unoCardName[15][8];
vector<vector<int>> playerCards;
bool unoDirection = true;
int unoCards[108] = { -1 };
bool hitDeal = false;
int hitDealCard = -1;
int lastId = -1;
bool uno[10] = {false};
int unoColor = 0;
bool unoColorChoose = false;
int unoLeftCards = 108;
vector<int> unoHaveHit;



/*
群成员信息
*/
string playerName(int64_t fromQQ)
{
	string name;
	switch (fromQQ) {
	case 283170421:		name = "小黑"; break;
	case 2543788018:		name = "小受"; break;
	case 593213294:		name = "繁星"; break;
	case 445312490:		name = "阿三"; break;
	case 348324261:		name = "豆奶"; break;
	case 916143191:		name = "一方"; break;
	case 806404355:		name = "鸣海"; break;
	case 1872859677:		name = "狗鱼王"; break;
	case 424068992:		name = "聪明"; break;
	case 444724229:		name = "卤蛋"; break;
	case 731032321:		name = "璃洛"; break;
	case 389568237:		name = "黄油猫"; break;
		
	default:			name = "";
	}
	return name;
}



/*
UNO GAME
*/
//重置
void unoReset()
{
	unoGame = false;
	for (int i = 0; i < 10; i++)
	{
		unoPlayer[i] = 0;
		uno[i] = false;
	}
	unoNum = 0;
	unoTurn = -1;
	unoCardTurn = 0;
	playerCards.clear();
	unoDirection = true;
	hitDeal = false;
	hitDealCard = -1;
	lastId = -1;
	unoColor = 0;
	unoColorChoose = false;
	unoLeftCards = 108;
	unoHaveHit.clear();
}
//洗牌
void unoDisorder(int a[], int n)
{
	int index, tmp, i;
	srand(time(NULL));
	for (i = 0; i < n; i++)
	{
		index = rand() % (n - i) + i;
		if (index != i)
		{
			tmp = a[i];
			a[i] = a[index];
			a[index] = tmp;
		}
	}
}
//生成牌组
void unoNameSet()
{
	string unoNameStr;
	for (int i = 0; i < 15; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (i < 13)
			{
				//颜色命名
				if (j < 2) { unoNameStr = "【红 "; }
				else if (j < 4) { unoNameStr = "【黄 "; }
				else if (j < 6) { unoNameStr = "【蓝 "; }
				else { unoNameStr = "【绿 "; }
				//数字命名
				if (i < 10) { unoNameStr += to_string(i) + "】"; }
				else if (i == 10) { unoNameStr += "跳】"; }
				else if (i == 11) { unoNameStr += "反】"; }
				else { unoNameStr += "+2】"; }
			}//功能牌命名
			else if (i == 13) { unoNameStr = "【Wild】"; }
			else { unoNameStr = "【+4】"; }
			unoCardName[i][j] = unoNameStr;
		}
	}
}
//对每一张牌命名
string unoName(int id)
{
	string name;
	if (id < 4)
	{
		name = unoCardName[0][id * 2];
	}
	else if (id < 104)
	{
		name = unoCardName[(id + 4) / 8][(id + 4) % 8];
	}
	else {
		name = unoCardName[14][0];
	}
	return name;
}
//设置场上颜色
int unoColorGet(int id)
{
	int color = -1;
	if (id < 4)
	{
		color = id + 1;
	}
	else if (id < 100)
	{
		color = (id + 4) % 8 / 2 + 1;
	}
	return color;
}
//显示顶层卡牌
void unoCall(int64_t group, int32_t id)
{
	unoTurn += unoDirection ? 1 : -1;
	if (unoTurn >= unoNum) { unoTurn = 0; }
	else if (unoTurn < 0) { unoTurn = unoNum - 1; }
	string unoHint = "=================\n现在顶层牌是";
	string color;
	if (id >= 100)
	{
		switch (unoColor)
		{
		case 1: color = "(红)"; break;
		case 2: color = "(黄)"; break;
		case 3: color = "(蓝)"; break;
		case 4: color = "(绿)"; break;
		default: color = "";
		}
	}
	else {
		color = "";
	}
	unoHint += unoName(id) + color + "\n=================\n";
	unoHint += "【轮到[CQ:at,qq=" + to_string(unoPlayer[unoTurn]) + "]出牌】";
	CQ_sendGroupMsg(ac, group, unoHint.c_str());
}

void unoStart(int64_t group)
{
	unoGame = true;
	string unoHint = "【Uno开始】";
	for (int i = 0; i < unoNum; i++)
	{
		unoHint += "[CQ:at,qq=" + to_string(unoPlayer[i]) + "]";
	}
	CQ_sendGroupMsg(ac, group, unoHint.c_str());

	//生成牌堆
	unoNameSet();
	for (int i = 0; i < 108; i++)
	{
		unoCards[i] = i;
	}
	unoDisorder(unoCards, 108);

	//每人初始发牌
	int unoStartNum;
	unoStartNum = unoNum < 6 ? 10 : 15 - unoNum;
	for (int i = 0; i < unoNum; i++)
	{
		unoHint = "【Uno游戏】游戏开始，你的初始手牌：\n";
		for (int j = i * unoStartNum; j < (i + 1) * unoStartNum; j++)
		{
			playerCards.push_back({});
			playerCards[i].push_back(unoCards[j]);
		}
		sort(playerCards[i].begin(), playerCards[i].end());
		for (auto& item : playerCards[i])
		{
			unoHint += unoName(item);
		}
		CQ_sendPrivateMsg(ac, unoPlayer[i], unoHint.c_str());
	}
	unoCardTurn += unoNum * unoStartNum;
	unoLeftCards -= unoNum * unoStartNum;

	//开始
	int index = unoCardTurn;
	while (unoCards[index] >= 100) { index++; }
	lastId = unoCards[index];
	unoLeftCards--;
	unoCall(group, lastId);
	unoCards[index] = -1;
}

void unoRefresh(int64_t group)
{
	unoLeftCards = 108;
	for (int i = 0; i < 108; i++)
	{
		unoCards[i] = i;
	}
	for (int j = 0; j < unoNum; j++)
	{
		for (auto& id : playerCards[j])
		{
			unoCards[id] = -1;
			unoLeftCards--;
		}
	}
	for (auto& idh : unoHaveHit)
	{
		unoCards[idh] = -1;
		unoLeftCards--;
	}
	unoHaveHit.clear();
	unoDisorder(unoCards, 108);
	unoCardTurn = 0;
	CQ_sendGroupMsg(ac, group, "【牌堆已重洗】");
}

void unoHit(int64_t group, int64_t fromQQ)
{
	string unoHint = "【Uno游戏】你抽到了";
	while (unoCards[unoCardTurn] == -1)
	{
		unoCardTurn++;
		if (unoCardTurn >= 108)
		{
			unoRefresh(group);
		}
	}
	unoLeftCards--;
	hitDealCard = unoCards[unoCardTurn];
	unoHaveHit.push_back(hitDealCard);
	unoHint += unoName(unoCards[unoCardTurn]) + "\n打出请在群内发送【**y】，不打请在群内发送【**n】";
	CQ_sendPrivateMsg(ac, fromQQ, unoHint.c_str());
	unoHint = "【" + playerName(fromQQ) + "抽了1张牌，请发送打出【**y】或不打【**n】】";
	CQ_sendGroupMsg(ac, group, unoHint.c_str());
	unoCardTurn++;
	if (unoLeftCards == 0)
	{
		unoRefresh(group);
	}
	unoHaveHit.clear();
	hitDeal = true;
}

void unoHitCards(int64_t group, string msg, int id = unoTurn)
{
	string submsg = msg.substr(3);
	string num = "";
	int hitNum;
	for (auto& str : submsg)
	{
		if (str != '0' && str != '1' && str != '2' && str != '3' && str != '4' && str != '5' && str != '6' && str != '7' && str != '8' && str != '9')
		{
			continue;
		}
		num += str;
	}
	if (num.size() == 0)
	{
		CQ_sendGroupMsg(ac, group, "【抽牌失败，字符无法识别】");
		return;
	}
	sscanf(num.c_str(), "%d", &hitNum);
	if (hitNum == 0)
	{
		CQ_sendGroupMsg(ac, group, "【抽牌失败，字符无法识别】");
		return;
	}
	int playerCardNum = 0;
	for (int k = 0; k < unoNum; k++)
	{
		playerCardNum += playerCards[k].size();
	}
	if (hitNum + playerCardNum > 108)
	{
		CQ_sendGroupMsg(ac, group, "【抽牌失败，抽牌数过多】");
		return;
	}

	vector<int> hit;
	for (int i = 0; i < hitNum; i++)
	{
		while (unoCards[unoCardTurn] == -1)
		{
			unoCardTurn++;
			if (unoCardTurn >= 108)
			{
				unoRefresh(group);
			}
		}
		hit.push_back(unoCards[unoCardTurn]);
		unoHaveHit.push_back(unoCards[unoCardTurn]);
		unoLeftCards--;
		unoCardTurn++;
		if (unoCardTurn >= 108)
		{
			unoRefresh(group);
		}
	}
	unoHaveHit.clear();
	for (auto& cid : hit)
	{
		playerCards[id].push_back(cid);
	}
	string unoHint = "【Uno游戏】抽牌完成，当前手牌：\n";
	sort(playerCards[id].begin(), playerCards[id].end());
	for (auto& item : playerCards[id])
	{
		unoHint += unoName(item);
	}
	CQ_sendPrivateMsg(ac, unoPlayer[id], unoHint.c_str());
	unoHint = "【" + playerName(unoPlayer[id]) + "抽了" + to_string(hitNum) + "张牌】";
	CQ_sendGroupMsg(ac, group, unoHint.c_str());
	uno[id] = false;
}

bool sameNum(int id1, int id2)
{
	if (id1 < 4)
	{
		return id2 < 4;
	}
	else if (id1 < 100)
	{
		int c = (id1 - 4) / 8;
		return id2 >= c * 8 + 4 && id2 < c * 8 + 12;
	}
	else {
		int c = (id1 - 100) / 4;
		return id2 >= c * 4 + 100 && id2 < c * 4 + 104;
	}
}

bool sameColor(int id1, int id2)
{
	return unoColorGet(id1) == unoColorGet(id2);
}

bool straightNum(vector<int> dealCards)
{
	if (dealCards.size() != 3) { return false; }
	if (!(dealCards[0] < dealCards[1] && dealCards[1] < dealCards[2])) { return false; }
	if (dealCards[2] > 75) { return false; }
	return ((dealCards[0] + 4) / 8 == (dealCards[1] + 4) / 8 - 1 && (dealCards[1] + 4) / 8 == (dealCards[2] + 4) / 8 - 1);
}

void unoHitDeal(int64_t group, bool deal)
{
	string unoHint = "【"+ playerName(unoPlayer[unoTurn]);
	if (deal == true)
	{
		int top_color = unoColorGet(hitDealCard);
		bool success = (sameNum(lastId, hitDealCard) || sameColor(lastId, hitDealCard) || (lastId >= 100 && unoColor == top_color) || top_color == -1);
		if (success == false) { CQ_sendGroupMsg(ac, group, "【出牌失败，出牌不符合规则】"); return; }
		unoHint += "出牌" + unoName(hitDealCard) + "】";
		CQ_sendGroupMsg(ac, group, unoHint.c_str());
		unoHint = "【Uno游戏】出牌完成，当前手牌：\n";
		for (auto& item : playerCards[unoTurn])
		{
			unoHint += unoName(item);
		}
		CQ_sendPrivateMsg(ac, unoPlayer[unoTurn], unoHint.c_str());
		if (hitDealCard >= 84 && hitDealCard < 92) { unoDirection = !unoDirection; }
		if (hitDealCard >= 76 && hitDealCard < 84)
		{
			unoTurn += unoDirection ? 1 : -1;
			if (unoTurn >= unoNum) { unoTurn = 0; }
			else if (unoTurn < 0) { unoTurn = unoNum - 1; }
		}
		lastId = hitDealCard;
		if (lastId >= 100)
		{
			unoColorChoose = true;
			unoHint = "【请选择颜色：红【**r】、黄【**y】、蓝【**b】、绿【**g】】";
			CQ_sendGroupMsg(ac, group, unoHint.c_str());
			hitDeal = false;
			return;
		}
	}
	else {
		unoHint += "选择不出牌】";
		CQ_sendGroupMsg(ac, group, unoHint.c_str());
		playerCards[unoTurn].push_back(hitDealCard);
		unoHint = "【Uno游戏】当前手牌：\n";
		sort(playerCards[unoTurn].begin(), playerCards[unoTurn].end());
		for (auto& item : playerCards[unoTurn])
		{
			unoHint += unoName(item);
		}
		CQ_sendPrivateMsg(ac, unoPlayer[unoTurn], unoHint.c_str());
		uno[unoTurn] = false;
	}
	hitDeal = false;
	unoCall(group, lastId);
}

void unoDealDo(int64_t group, int64_t fromQQ, int turn = unoTurn)
{
	string unoHint = "【Uno游戏】出牌完成，当前手牌：\n";
	for (auto& item : playerCards[turn])
	{
		unoHint += unoName(item);
	}
	CQ_sendPrivateMsg(ac, fromQQ, unoHint.c_str());
	unoColorChoose = false;
	unoCall(group, lastId);
}

void unoDeal(string msg, int64_t group, int64_t fromQQ)
{
	int count = -1;
	int strGet[8][2] = { { -1,-1 },{ -1,-1 },{ -1,-1 },{ -1,-1 },{ -1,-1 },{ -1,-1 },{ -1,-1 },{ -1,-1 } };
	int get = 0;
	bool afterSpace = false;
	bool success = true;

	//识别出牌
	for (auto& str : msg)
	{
		count++;
		if (count < 2) { continue; }
		if (get > 15) { success = false; break; }
		if (str == ' ') { afterSpace = true; continue; }
		if (str == 'h')
		{
			if (msg == "** hit")
			{
				unoHit(group, fromQQ);
			}
			else {
				unoHitCards(group, msg);
			}
			return;
		}
		if (get % 2 == 0)
		{
			if (afterSpace == true || count == 2)
			{
				switch (str)
				{
				case 'R': case 'r': case '1': strGet[get / 2][1] = 0; break;
				case 'Y': case 'y': case '2': strGet[get / 2][1] = 2; break;
				case 'B': case 'b': case '3': strGet[get / 2][1] = 4; break;
				case 'G': case 'g': case '4': strGet[get / 2][1] = 6; break;
				case 'W': case 'w': strGet[get / 2][1] = 0; strGet[get / 2][0] = 13; get++; break;
				case 'X': case 'x': case '+': strGet[get / 2][1] = 0; strGet[get / 2][0] = 14; get++; break;
				default: success = false;
				}
				if (success == false) { break; }
			}
			else { afterSpace = false; continue; }
		}
		else {
			switch (str)
			{
			case '0': strGet[(get - 1) / 2][0] = 0; break;
			case '1': strGet[(get - 1) / 2][0] = 1; break;
			case '2': strGet[(get - 1) / 2][0] = 2; break;
			case '3': strGet[(get - 1) / 2][0] = 3; break;
			case '4': strGet[(get - 1) / 2][0] = 4; break;
			case '5': strGet[(get - 1) / 2][0] = 5; break;
			case '6': strGet[(get - 1) / 2][0] = 6; break;
			case '7': strGet[(get - 1) / 2][0] = 7; break;
			case '8': strGet[(get - 1) / 2][0] = 8; break;
			case '9': strGet[(get - 1) / 2][0] = 9; break;
			case 'S': case 's': strGet[(get - 1) / 2][0] = 10; break;
			case 'R': case 'r': strGet[(get - 1) / 2][0] = 11; break;
			case 'X': case 'x': case '+': strGet[(get - 1) / 2][0] = 12; break;
			default: success = false;
			}
			if (success == false) { break; }
		}
		get++;
		afterSpace = false;
	}
	if (get % 2 == 1) { success = false; }
	if (success == false || strGet[0][0] == -1) { CQ_sendGroupMsg(ac, group, "【出牌失败，字符无法识别】"); return; }

	//检查手牌
	set<int> delSet;
	vector<int> dealCards;
	int turn = unoTurn;
	int index;
	int lastIDNow = lastId;
	for (int i = 0;i < 8; i++)
	{
		if (strGet[i][0] == -1) { break; }
		success = false;
		for (auto& id : playerCards[turn])
		{
			if (delSet.find(id) != delSet.end()) { continue; }
			if (unoName(id) == unoCardName[strGet[i][0]][strGet[i][1]])
			{
				delSet.insert(id);
				dealCards.push_back(id);
				success = true;
				lastIDNow = id;
				break;
			}
		}
		if (success == false) { break; }
	}
	if (success == false) { CQ_sendGroupMsg(ac, group, "【出牌失败，只能打手牌里的牌】"); return; }
	if (delSet.size() == playerCards[turn].size() && lastIDNow > 75) { CQ_sendGroupMsg(ac, group, "【出牌失败，最后一打必须是数字牌】"); return; }

	int check_type = -1;
	if (dealCards.size() > 1)
	{
		for (int i = 0; i < dealCards.size(); i++)
		{
			if (i == 0) { continue; }
			if (sameNum(dealCards[0], dealCards[i])) { check_type = 0; }
			else { success = false; break; }
		}
	}
	if (dealCards.size() == 3 && check_type != 0)
	{
		if (straightNum(dealCards)) { success = true; }
	}
	if (success == true)
	{
		int top_color = unoColorGet(dealCards[0]);
		success = (sameNum(lastId, dealCards[0]) || sameColor(lastId, dealCards[0]) || (lastId >= 100 && unoColor == top_color) || top_color == -1);
	}
	if (success == false) { CQ_sendGroupMsg(ac, group, "【出牌失败，出牌不符合规则】"); return; }

	lastId = lastIDNow;
	if (lastId >= 100) { unoColorChoose = true; }
	for (auto itor = playerCards[turn].begin(); itor != playerCards[turn].end();)
	{
		if (delSet.count(*itor))
		{
			if (*itor > 83 && *itor <= 91) { unoDirection = !unoDirection; }
			if (*itor > 75 && *itor <= 83)
			{
				unoTurn += unoDirection ? 1 : -1;
				if (unoTurn >= unoNum) { unoTurn = 0; }
				else if (unoTurn < 0) { unoTurn = unoNum - 1; }
			}
			itor = playerCards[turn].erase(itor);
		}
		else {
			++itor;
		}
	}

	//通知
	string unoHint;
	unoHint = playerName(fromQQ) + "出牌";
	for (int i = 0; i < 8; i++)
	{
		if (strGet[i][0] == -1) { break; }
		unoHint += unoCardName[strGet[i][0]][strGet[i][1]];
	}
	CQ_sendGroupMsg(ac, group, unoHint.c_str());
	if (playerCards[turn].size() == 0)
	{
		unoHint = "【游戏结束";
		unoHint += playerName(fromQQ) == "" ? "】" : "，" + playerName(fromQQ) + "获胜】";
		CQ_sendGroupMsg(ac, group, unoHint.c_str());
		CQ_sendPrivateMsg(ac, fromQQ, "【Uno游戏】出牌完成，你获胜了！");
		unoReset();
		return;
	}
	else {
		if (unoColorChoose == true)
		{
			unoHint = "【请选择颜色：红【**r】、黄【**y】、蓝【**b】、绿【**g】】";
			CQ_sendGroupMsg(ac, group, unoHint.c_str());
			return;
		}
		else {
			unoDealDo(group, fromQQ, turn);
		}
	}
}

void unoInfo(int64_t group)
{
	string player = "";
	string name;
	int index = 0;
	for (auto& qq : unoPlayer)
	{
		if (qq == 0) { break; }
		name = playerName(qq);
		if (name == "") { name = "[CQ:at,qq=" + to_string(qq) + "]"; }
		int plus = (index == unoTurn && hitDeal == true) ? 1 : 0;
		if (unoGame == true) { name += "(" + to_string(playerCards[index].size() + plus) + ")"; }
		if (unoDirection == true)
		{
			player += name;
			if (index < unoNum - 1) { player += " → "; }
		}
		else {
			player = name + player;
			if (index < unoNum - 1) { player = " → " + player; }
		}
		index++;
	}
	if (unoGame == false)
	{
		CQ_sendGroupMsg(ac, group, ("【Uno游戏】游戏尚未开始，现在报名的玩家：\n" + player).c_str());
	}
	else {
		string top = unoName(lastId);
		string who = playerName(unoPlayer[unoTurn]);
		string left = to_string(unoLeftCards);
		string unoHint = "=====【Uno游戏】=====";
		vector<int> needHit;
		string color;
		if (lastId >= 100)
		{
			switch (unoColor)
			{
			case 1: color = "(红)"; break;
			case 2: color = "(黄)"; break;
			case 3: color = "(蓝)"; break;
			case 4: color = "(绿)"; break;
			default: color = "";
			}
		}
		else {
			color = "";
		}
		if (unoColorChoose == true) { color = ""; }
		unoHint += "\n顶层牌：" + top + color;
		unoHint += "\n当前出牌：" + who;
		unoHint += "\n牌堆剩余：" + left + "张";
		unoHint += "\n当前出牌顺序及手牌数：\n" + player;
		for (int i = 0; i < unoNum; i++)
		{
			if (uno[i] == false && playerCards[i].size() == 1)
			{
				name = playerName(unoPlayer[i]);
				if (name == "") { name = "[CQ:at,qq=" + to_string(unoPlayer[i]) + "]"; }
				unoHint += "\n\n" + name + "没有宣言UNO";
				needHit.push_back(i);
			}
			if (uno[i] == true && playerCards[i].size() != 1 && i != unoTurn)
			{
				name = playerName(unoPlayer[i]);
				if (name == "") { name = "[CQ:at,qq=" + to_string(unoPlayer[i]) + "]"; }
				unoHint += "\n\n" + name + "进行了假的UNO宣言";
				needHit.push_back(i);
			}
		}
		unoHint += "\n===================";
		CQ_sendGroupMsg(ac, group, unoHint.c_str());
		for (auto& id : needHit)
		{
			unoHitCards(group, "**hit1", id);
		}
	}
}



void play_24(int64_t group)
{
	int num[4];
	string str = "";
	str += "【算24点】\n【 ";
	srand((unsigned)time(NULL));
	for (int i = 0; i < 4; i++)
	{
		num[i] = rand() % 13 + 1;
		switch (num[i])
		{
		case 11:	str += "J"; break;
		case 12:	str += "Q"; break;
		case 13:	str += "K"; break;
		default:	str += to_string(num[i]);
		}
		if (i < 3) { str += ", "; }
	}
	str += " 】";
	CQ_sendGroupMsg(ac, group, str.c_str());
}



/*
MSG
*/

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

			//检测和判断
			string unoHint;
			bool unoInclude = false;
			if (msg.msg.substr(0, 1) == "*")
			{
				if (msg.msg == "*uno join")
				{
					if (unoGame == false)
					{
						//报名的玩家是否已报名
						for (int i = 0; i < 10; i++)
						{
							if (unoPlayer[i] == msg.fromQQ)
							{
								unoInclude = true;
								break;
							}
							unoInclude = false;
						}

						if (unoInclude)
						{
							CQ_sendGroupMsg(ac, msg.fromGroup, "【报名失败，你已报名】");
						}
						else if (unoNum >= 10)
						{
							CQ_sendGroupMsg(ac, msg.fromGroup, "【报名失败，玩家数已满】");
						}
						else {
							unoNum++;
							unoHint = "【报名成功，当前有" + to_string(unoNum) + "名玩家】";
							CQ_sendGroupMsg(ac, msg.fromGroup, unoHint.c_str());
							unoPlayer[unoNum - 1] = msg.fromQQ;
						}
					}
					else {
						CQ_sendGroupMsg(ac, msg.fromGroup, "【报名失败，游戏已经开始】");
					}

				}
				else if (msg.msg == "*uno quit")
				{
					//检测玩家是否已报名
					int unoIndex = -1;
					for (int i = 0; i < 10; i++)
					{
						if (unoPlayer[i] == msg.fromQQ)
						{
							unoInclude = true;
							unoIndex = i;
							break;
						}
						unoInclude = false;
					}
					if (unoGame == true)
					{
						CQ_sendGroupMsg(ac, msg.fromGroup, "【取消报名失败，游戏已经开始】");
					}
					else if (unoInclude)
					{
						for (int i = unoIndex; i < 10; i++)
						{
							unoPlayer[i] = i == 9 ? 0 : unoPlayer[i + 1];
						}
						unoNum--;
						unoHint = "【取消报名成功，当前有" + to_string(unoNum) + "名玩家】";
						CQ_sendGroupMsg(ac, msg.fromGroup, unoHint.c_str());
					}
					else {
						CQ_sendGroupMsg(ac, msg.fromGroup, "【取消报名失败，你没有报名】");
					}
				}

				else if (msg.msg == "*uno start")
				{
					if (unoGame == false)
					{
						if (unoNum < 2)
						{
							CQ_sendGroupMsg(ac, msg.fromGroup, "【开始失败，玩家不足】");
						}
						else {
							unoStart(msg.fromGroup);
						}
					}
					else {
						CQ_sendGroupMsg(ac, msg.fromGroup, "【开始失败，游戏已经开始】");
					}
				}
				else if (msg.msg == "*uno reset")
				{
					CQ_sendGroupMsg(ac, msg.fromGroup, "【Uno重置完成】");
					unoReset();
				}
				else if (msg.msg.substr(0, 2) == "**")
				{
					if (unoGame == true)
					{
						if (unoPlayer[unoTurn] == msg.fromQQ)
						{
							if (hitDeal == true)
							{
								if (msg.msg == "**y" || msg.msg == "**Y" || msg.msg == "**1") { unoHitDeal(msg.fromGroup, true); }
								else if (msg.msg == "**n" || msg.msg == "**N" || msg.msg == "**0") { unoHitDeal(msg.fromGroup, false); }
							}
							else if (unoColorChoose == true)
							{
								if (msg.msg == "**r") { unoColor = 1; unoDealDo(msg.fromGroup, msg.fromQQ); }
								else if (msg.msg == "**y") { unoColor = 2; unoDealDo(msg.fromGroup, msg.fromQQ); }
								else if (msg.msg == "**b") { unoColor = 3; unoDealDo(msg.fromGroup, msg.fromQQ); }
								else if (msg.msg == "**g") { unoColor = 4; unoDealDo(msg.fromGroup, msg.fromQQ); }
							}
							else {
								if (msg.msg == "**hit")
								{
									unoHit(msg.fromGroup, msg.fromQQ);
								}
								else {
									unoDeal(msg.msg, msg.fromGroup, msg.fromQQ);
								}
							}
						}
						else {
							CQ_sendGroupMsg(ac, msg.fromGroup, "【你未参加游戏，或尚未轮到你出牌】");
						}
					}
					else {
						CQ_sendGroupMsg(ac, msg.fromGroup, "【游戏尚未开始】");
					}
				}
				else if (msg.msg == "*info")
				{
					unoInfo(msg.fromGroup);
				}
				else if (msg.msg == "*uno")
				{
					if (unoGame == false)
					{
						CQ_sendGroupMsg(ac, msg.fromGroup, "【游戏尚未开始】");
					}
					else {
						if (unoPlayer[unoTurn] == msg.fromQQ)
						{
							uno[unoTurn] = true;
							unoHint = "进行了UNO宣言】";
							unoHint = "【" + playerName(msg.fromQQ) + unoHint;
							CQ_sendGroupMsg(ac, msg.fromGroup, unoHint.c_str());
						}
						else {
							CQ_sendGroupMsg(ac, msg.fromGroup, "【你未参加游戏，或尚未轮到你出牌】");
						}
					}
				}
				else if (msg.msg == "*uno rule")
				{
					CQ_sendGroupMsg(ac, msg.fromGroup, "【Uno游戏规则】\nUno游戏为卡牌游戏，拥有红黄蓝绿四种颜色的牌，\n包括数字0-9的牌，及跳过牌、反转牌、+2牌。\n此外，还有无色的变色牌（Wild）和+4牌。\n游戏开始时，每人会在私聊发到一定数量的牌，然后轮流出牌。\n根据当前顶层牌，可以出与其同色的牌，也可以出与其同内容的牌。当无牌可打时，必须抽一张牌，然后选择是否打出这张牌。\n每当打出一张跳过牌，下一个人便被跳过一回合。\n每当打出一张反转牌，出牌顺序就反过来。\n每当打出一张+2牌，下一个人就必须摸2张牌，或者打出+2或+4牌来避免摸牌。\n变色牌和+4牌是无色的，必须在打出后决定它的颜色。\n每当打出一张+4牌，下一个人就必须摸4张牌，或者打出+4或同色的+2牌来避免摸牌。\n+2牌和+4牌可以累加摸牌数，直到有人摸牌。\n允许一次性打出多张相同内容的牌，如红2+绿2，第一张需要与顶层牌同色或同内容。\n允许一次性打出3张相连的数字牌，如红0+黄1+黄2，第一张需要与顶层牌同色或同内容。\n以上两条，都是以最后一张为新的顶层牌。另外，在打完手牌前，最后一打必须是数字牌。");
				}
				else if (msg.msg == "*24")
				{
					play_24(msg.fromGroup);
				}
				else if (msg.msg == "*help")
				{
					CQ_sendGroupMsg(ac, msg.fromGroup, "【Uno游戏出牌方法】【**】出牌\n红色：r / R / 0\n黄色：y / Y / 1\n蓝色：b / B / 2\n绿色：g / G / 3\n数字：0-9\n跳过牌：s / S\n反转牌：r / R\n+2牌：x / X / +\n变色牌：w / W\n+4牌：x / X / +\n（牌之间用空格隔开）\n示例：\n** r2 g2\n打出红2+绿2\n** +\n打出+4\n【**hit】抽1张牌（然后决定是否打出）\n【**hitX】抽X张牌");
				}
				else if (msg.msg == "*menu")
				{
					CQ_sendGroupMsg(ac, msg.fromGroup, "【*menu】查看指令菜单\n【*info】查看游戏信息\n【*help】查看游戏帮助\n【*uno rule】查看Uno游戏规则\n【*uno join】Uno报名\n【*uno quit】Uno取消报名\n【*uno start】Uno开始\n【*uno reset】Uno重置\n【*uno】UNO宣言\n【**】出牌\n【**hit】抽1张牌\n【**hitX】抽X张牌\n【*24】算24点");
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