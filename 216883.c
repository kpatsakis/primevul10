	Kick(User* source, Membership* memb, const std::string& reason)
		: ClientProtocol::Message("KICK", source)
	{
		PushParamRef(memb->chan->name);
		PushParamRef(memb->user->nick);
		PushParamRef(reason);
	}