	Privmsg(NoCopy, User* source, const User* targetuser, const std::string& text, MessageType mt = MSG_PRIVMSG)
		: ClientProtocol::Message(CommandStrFromMsgType(mt), source)
	{
		PushTargetUser(targetuser);
		PushParamRef(text);
	}