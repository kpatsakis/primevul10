	Numeric(unsigned int num)
		: ClientProtocol::Message(NULL, ServerInstance->Config->GetServerName())
	{
		InitCommand(num);
		PushParam("*");
	}