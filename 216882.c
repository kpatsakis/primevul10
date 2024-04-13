	void SetParams(Membership* Memb)
	{
		memb = Memb;
		PushParamRef(memb->chan->name);
	}