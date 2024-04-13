	Mode(User* source, Channel* Chantarget, User* Usertarget, const Modes::ChangeList& changelist)
		: ClientProtocol::Message("MODE", source)
		, chantarget(Chantarget)
		, usertarget(Usertarget)
		, beginit(changelist.getlist().begin())
	{
		PushParamRef(GetStrTarget());
		PushParam(ToModeLetters(changelist.getlist(), 450, beginit, lastit));
		PushModeParams();
	}