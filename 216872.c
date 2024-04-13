	Numeric(const ::Numeric::Numeric& num, const std::string& target)
		: ClientProtocol::Message(NULL, (num.GetServer() ? num.GetServer() : ServerInstance->FakeClient->server)->GetName())
	{
		PushParamRef(target);
		InitFromNumeric(num);
	}