
//����һ��������QӦ�õĳ����¼�
struct Event {

	//�¼�����
	enum class Type {
		//��Q����/�ر�
		startup,
		shutdown,
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
