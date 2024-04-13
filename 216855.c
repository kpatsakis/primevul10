	Nick(User* source, const std::string& newnick)
		: ClientProtocol::Message("NICK", source)
	{
		PushParamRef(newnick);
	}