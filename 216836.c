	Numeric(const ::Numeric::Numeric& num, User* user)
		: ClientProtocol::Message(NULL, (num.GetServer() ? num.GetServer() : ServerInstance->FakeClient->server)->GetName())
	{
		if (user->registered & REG_NICK)
			PushParamRef(user->nick);
		else
			PushParam("*");
		InitFromNumeric(num);
	}