	Part(Membership* memb, const std::string& reason)
		: ClientProtocol::Message("PART", memb->user)
	{
		PushParamRef(memb->chan->name);
		if (!reason.empty())
			PushParamRef(reason);
	}