
//����һ��������QӦ�õĳ����¼�
struct Event {

	//�¼�����
	enum class Type {
		enabled,
		disabled,
		setAuthCode,
		groupMessage
	};

	Event(Type type)
		:type(type) {}

	Type type;
};
