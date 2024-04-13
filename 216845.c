	Mode()
		: ClientProtocol::Message("MODE", ServerInstance->FakeClient)
		, chantarget(NULL)
		, usertarget(NULL)
	{
	}