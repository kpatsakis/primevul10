	Error(const std::string& text)
			: ClientProtocol::Message("ERROR")
	{
		PushParam(text);
	}