	Quit(User* source, const std::string& reason)
		: ClientProtocol::Message("QUIT", source)
	{
		if (!reason.empty())
			PushParamRef(reason);
	}