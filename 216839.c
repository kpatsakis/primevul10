	Privmsg(User* source, const Channel* targetchan, const std::string& text, MessageType mt = MSG_PRIVMSG, char status = 0)
		: ClientProtocol::Message(CommandStrFromMsgType(mt), source)
	{
		PushTargetChan(status, targetchan);
		PushParam(text);
	}