	Join(Membership* Memb, const std::string& sourcestrref)
		: ClientProtocol::Message("JOIN", sourcestrref, Memb->user)
	{
		SetParams(Memb);
	}