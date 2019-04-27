#include "MsgSub.h"
#include "cqp.h"
#include <time.h>
#include <string>
using namespace std;

//����ȫ�ֱ��� ac   ��appmain.h �ж���
extern int ac;


int players = 0;			//�����
bool bombGame = false;		 //�ж���Ϸ�Ƿ�ʼ�ı�־
int64_t bombPlayer[10] = { 0 };//�μ���Ϸ�����
int bombNumber = -1;			//ը�����֣���ʼ��Ϊ-1
int bombTurn = -1;			//�����غ�
int low = 1;		//��ʼ�����½�
int high = 100;

//��Ϸ��ʼ
void bombStart(int64_t group)
{
	bombGame = true;
	srand((unsigned)time(NULL));
	bombNumber = rand() % (99 - 2 + 1) + 2;	//����[2,99]�������
	low = 1;
	high = 100;
	bombTurn = 0;
	string bombHint = "������~��������ը������Ϸ��ʼ\n�����Ѿ�����~\n��1������>��100��";
	for (int i = 0; i < players; i++)
	{
		bombHint += "[CQ:at,qq=" + to_string(bombPlayer[i]) + "]";
	}
	CQ_sendGroupMsg(ac, group, bombHint.c_str());
}
//��Ϸ����
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

//��������㷨
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

//ը���ж�
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
		CQ_sendGroupMsg(ac, group, "�����ֳ�����Χ�����������룡��");
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

			string bombHint;			//������Ϸ��ʾ
			bool bombInclude = false;	//�������Ƿ��ѱ���
			//����һ���ַ���*ʱ��ȡָ��
			if (msg.msg.substr(0, 1) == "*")	
			{
				if (msg.msg == "*bomb join")
				{
					//����Ϸδ��ʼʱ
					if (bombGame == false) {

						//�������Ƿ��ѱ���
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
							CQ_sendGroupMsg(ac, msg.fromGroup, "������ʧ�ܣ����ѱ�����");
						}
						else if (players >= 10)
						{
							CQ_sendGroupMsg(ac, msg.fromGroup, "������ʧ�ܣ������������");
						}
						else
						{
							players++;
							bombHint = "�������ɹ�����ǰ��" + to_string(players) + "λ��ҡ�";
							CQ_sendGroupMsg(ac, msg.fromGroup, bombHint.c_str());
							bombPlayer[players - 1] = msg.fromQQ;	//��������Ϣ��QQ��������
						}
					}
					else {
						CQ_sendGroupMsg(ac, msg.fromGroup, "������ʧ�ܣ���Ϸ�Ѿ���ʼ��");
					}
				}
				else if (msg.msg == "*bomb quit")
				{
					int bombIndex = -1;
					//�������Ƿ��ѱ���
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
						CQ_sendGroupMsg(ac, msg.fromGroup, "��ȡ������ʧ�ܣ���Ϸ�Ѿ���ʼ��");
					}
					else if (bombInclude)
					{
						//����ȡ����������Һ����ÿ�������ǰ�ƶ�һλ
						for (int i = bombIndex; i < 10; i++)
						{
							bombPlayer[i] = i == 9 ? 0 : bombPlayer[i + 1];
						}
						players--;
						bombHint = "��ȡ�������ɹ�����ǰ��" + to_string(players) + "����ҡ�";
						CQ_sendGroupMsg(ac, msg.fromGroup, bombHint.c_str());
					}
					else {
						CQ_sendGroupMsg(ac, msg.fromGroup, "��ȡ������ʧ�ܣ���û�б�����");
					}
				}

				else if (msg.msg == "*bomb start")
				{
					if (bombGame == false) {
						if (players < 1)
						{
							CQ_sendGroupMsg(ac, msg.fromGroup, "����ʼʧ�ܣ���Ҳ��㡿");
						}
						else {
							Disorder(bombPlayer, players);
							bombStart(msg.fromGroup);
							bombHint = "��������[CQ:at,qq=" + to_string(bombPlayer[bombTurn]) + "]������";
							CQ_sendGroupMsg(ac, msg.fromGroup, bombHint.c_str());
						}
					}
					else {
						CQ_sendGroupMsg(ac, msg.fromGroup, "����ʼʧ�ܣ���Ϸ�Ѿ���ʼ��");
					}
				}
				else if (msg.msg == "*bomb reset")
				{
					bombReset();
					CQ_sendGroupMsg(ac, msg.fromGroup, "������ը��������ɡ�");
				}
				else if (msg.msg.substr(0, 3) == "**b")
				{
					if (bombGame == true) {
						if (bombPlayer[bombTurn] == msg.fromQQ)
						{
							if (bombJudge(msg.fromGroup, msg.msg))
							{
								bombHint = "���ء�����������������\n[CQ:at,qq=" + to_string(bombPlayer[bombTurn]) + "]\n������1����\n��Ϸ����";
								CQ_setGroupBan(ac, msg.fromGroup, msg.fromQQ, 60);//����60s
								CQ_sendGroupMsg(ac, msg.fromGroup, bombHint.c_str());
								bombReset();
							}
							else
							{
								bombTurn++;
								//���ѭ��
								if (bombTurn == players) {
									bombTurn = 0;
								}
								bombHint = "��" + to_string(low) + "������>��" + to_string(high) + "��\n";
								bombHint += "���ֵ�[CQ:at,qq=" + to_string(bombPlayer[bombTurn]) + "]������";
								CQ_sendGroupMsg(ac, msg.fromGroup, bombHint.c_str());
							}
						}
						else {
							CQ_sendGroupMsg(ac, msg.fromGroup, "����δ�μ���Ϸ������δ�ֵ���Ļغϡ�");
						}
					}
					else if(bombGame == false)
					{
						CQ_sendGroupMsg(ac, msg.fromGroup, "����Ϸ��δ��ʼ��");
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



