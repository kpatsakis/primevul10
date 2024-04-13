	Invite(User* source, User* target, Channel* chan)
		: ClientProtocol::Message("INVITE", source)
	{
		PushParamRef(target->nick);
		PushParamRef(chan->name);
	}