	Ping(const std::string& cookie)
		: ClientProtocol::Message("PING")
	{
		PushParamRef(cookie);
	}