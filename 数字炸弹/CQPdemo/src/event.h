
//代表一个发给酷Q应用的抽象事件
struct Event {

	//事件类型
	enum class Type {
		//酷Q启动/关闭
		startup,
		shutdown,
		//插件启动/关闭
		enabled,
		disabled,
		//设置AuthCode
		setAuthCode,
		//收到群聊
		groupMessage
	};

	Event(Type type)
		:type(type) {}

	Type type;
};
