	static const char* CommandStrFromMsgType(MessageType mt)
	{
		return ((mt == MSG_PRIVMSG) ? "PRIVMSG" : "NOTICE");
	}