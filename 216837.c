	Join(Membership* Memb)
		: ClientProtocol::Message("JOIN", Memb->user)
	{
		SetParams(Memb);
	}