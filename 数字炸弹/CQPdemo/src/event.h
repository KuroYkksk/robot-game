
//代表一个发给酷Q应用的抽象事件
struct Event {

	//事件类型
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
