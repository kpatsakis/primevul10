	Ping()
		: ClientProtocol::Message("PING")
	{
		PushParamRef(ServerInstance->Config->GetServerName());
	}