	Privmsg(const std::string& source, const std::string& target, const std::string& text, MessageType mt = MSG_PRIVMSG, char status = 0)
		: ClientProtocol::Message(CommandStrFromMsgType(mt), source)
	{
		if (status)
		{
			std::string rawtarget(1, status);
			rawtarget.append(target);
			PushParam(rawtarget);
		}
		else
		{
			PushParam(target);
		}
		PushParam(text);
	}