	Privmsg(NoCopy, User* source, const std::string& target, const std::string& text, MessageType mt = MSG_PRIVMSG)
		: ClientProtocol::Message(CommandStrFromMsgType(mt), source)
	{
		PushParam(target);
		PushParamRef(text);
	}