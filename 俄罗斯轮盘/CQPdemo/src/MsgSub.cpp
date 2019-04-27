#include "MsgSub.h"
#include "cqp.h"
#include <time.h>
#include <string>
using namespace std;

//����ȫ�ֱ��� ac   ��appmain.h �ж���
extern int ac;


int players = 0;			//�����
bool ruroGame = false;		 //�ж���Ϸ�Ƿ�ʼ�ı�־
int64_t ruroPlayer[6] = { 0 };//�μ����̵����
int bullet = -1;			//�ӵ���λ�ã���ʼ��Ϊ-1
int shootTurn = -1;			//��ǹ�غ�

//��Ϸ��ʼ
void ruroStart(int64_t group)
{
	ruroGame = true;
	srand((unsigned)time(NULL));
	bullet = rand() % 6;	//����[1,6]�������
	shootTurn = 0;
	string ruroHint = "������~���Զ���˹���̡���Ϸ��ʼ\n1���ӵ���װ��~\n";
	for (int i = 0; i < players; i++)
	{
		ruroHint += "[CQ:at,qq=" + to_string(ruroPlayer[i]) + "]";
	}
	CQ_sendGroupMsg(ac, group, ruroHint.c_str());
}
//��Ϸ����
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

			string ruroHint;			//������Ϸ��ʾ
			bool ruroInclude = false;	//�������Ƿ��ѱ���
			//����һ���ַ���*ʱ��ȡָ��
			if (msg.msg.substr(0, 1) == "*")	
			{
				if (msg.msg == "*ruro join")
				{
					//����Ϸδ��ʼʱ
					if (ruroGame == false) {

						//�������Ƿ��ѱ���
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
							CQ_sendGroupMsg(ac, msg.fromGroup, "������ʧ�ܣ����ѱ�����");
						}
						else if (players >= 6)
						{
							CQ_sendGroupMsg(ac, msg.fromGroup, "������ʧ�ܣ������������");
						}
						else
						{
							players++;
							ruroHint = "�������ɹ�����ǰ��" + to_string(players) + "λ��ҡ�";
							CQ_sendGroupMsg(ac, msg.fromGroup, ruroHint.c_str());
							ruroPlayer[players - 1] = msg.fromQQ;	//��������Ϣ��QQ��������
						}
					}
					else {
						CQ_sendGroupMsg(ac, msg.fromGroup, "������ʧ�ܣ���Ϸ�Ѿ���ʼ��");
					}
				}
				else if (msg.msg == "*ruro quit")
				{
					int ruroIndex = -1;
					//�������Ƿ��ѱ���
					for (int i = 0; i < 6; i++)
					{
						if (ruroPlayer[i] == msg.fromQQ)
						{
							ruroInclude = true;
							int ruroIndex = -1;
							break;
						}
						ruroInclude = false;
					}

					if (ruroGame == true)
					{
						CQ_sendGroupMsg(ac, msg.fromGroup, "��ȡ������ʧ�ܣ���Ϸ�Ѿ���ʼ��");
					}
					else if (ruroInclude)
					{
						//����ȡ����������Һ����ÿ�������ǰ�ƶ�һλ
						for (int i = ruroIndex; i < 6; i++)
						{
							ruroPlayer[i] = i == 5 ? 0 : ruroPlayer[i + 1];
						}
						players--;
						ruroHint = "��ȡ�������ɹ�����ǰ��" + to_string(players) + "����ҡ�";
						CQ_sendGroupMsg(ac, msg.fromGroup, ruroHint.c_str());
					}
					else {
						CQ_sendGroupMsg(ac, msg.fromGroup, "��ȡ������ʧ�ܣ���û�б�����");
					}
				}

				else if (msg.msg == "*ruro start")
				{
					if (ruroGame == false) {
						if (players < 1)
						{
							CQ_sendGroupMsg(ac, msg.fromGroup, "����ʼʧ�ܣ���Ҳ��㡿");
						}
						else {
							Disorder(ruroPlayer, players);
							ruroStart(msg.fromGroup);
							ruroHint = "��������[CQ:at,qq=" + to_string(ruroPlayer[shootTurn]) + "]��ǹ��";
							CQ_sendGroupMsg(ac, msg.fromGroup, ruroHint.c_str());
						}
					}
					else {
						CQ_sendGroupMsg(ac, msg.fromGroup, "����ʼʧ�ܣ���Ϸ�Ѿ���ʼ��");
					}
				}
				else if (msg.msg == "*ruro reset")
				{
					ruroReset();
					CQ_sendGroupMsg(ac, msg.fromGroup, "������˹����������ɡ�");
				}
				else if (msg.msg.substr(0, 2) == "**")
				{
					if (ruroGame == true) {
						if (ruroPlayer[shootTurn] == msg.fromQQ)
						{
							if (msg.msg == "**shoot")
							{
								bullet--;
								shootTurn++;
								if (bullet == 0) {
									ruroHint = "���顪����������������\n[CQ:at,qq=" + to_string(ruroPlayer[shootTurn-1]) + "]\n������1����\n��Ϸ����";
									CQ_setGroupBan(ac, msg.fromGroup, msg.fromQQ, 60);//����60s
									CQ_sendGroupMsg(ac, msg.fromGroup, ruroHint.c_str());
									ruroReset();
								}
								else if (shootTurn == players) {
									CQ_sendGroupMsg(ac, msg.fromGroup, "��������Ҵ��~~��Ϸ������");
									ruroReset();
								}
								else if (shootTurn != players) {
									CQ_sendGroupMsg(ac, msg.fromGroup, "��ʲô��û����~��");
									ruroHint = "���ֵ�[CQ:at,qq=" + to_string(ruroPlayer[shootTurn]) + "]��ǹ��";
									CQ_sendGroupMsg(ac, msg.fromGroup, ruroHint.c_str());
								}
							}
						}
						else {
							CQ_sendGroupMsg(ac, msg.fromGroup, "����δ�μ���Ϸ������δ�ֵ���Ļغϡ�");
						}
					}
					else {
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
	//����������Ϣ����100��  ��ֱ�������µ�����Ϣ
	if (m_groupMsgBuffer.size() < 100)
	{
		m_mutex.lock();
		m_groupMsgBuffer.push(GroupMsg(msgId, fromGroup, fromQQ, msg));
		m_mutex.unlock();
	}
}


