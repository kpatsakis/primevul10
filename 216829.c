	Topic(User* source, const Channel* chan, const std::string& newtopic)
		: ClientProtocol::Message("TOPIC", source)
	{
		PushParamRef(chan->name);
		PushParamRef(newtopic);
	}