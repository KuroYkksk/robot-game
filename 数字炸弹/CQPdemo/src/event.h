#pragma once
//����һ��������QӦ�õĳ����¼�
struct Event {

	//�¼�����
	enum class Type {
		//��Q����/�ر�
		init,
		exit,
		//�������/�ر�
		enabled,
		disabled,
		//����AuthCode
		setAuthCode,
		//�յ�Ⱥ��
		groupMessage
	};

	Event(Type type)
		:type(type) {}

	Type type;
};
