	Mode(User* source, Channel* Chantarget, User* Usertarget, const Modes::ChangeList& changelist, Modes::ChangeList::List::const_iterator beginiter)
		: ClientProtocol::Message("MODE", source)
		, chantarget(Chantarget)
		, usertarget(Usertarget)
		, beginit(beginiter)
	{
		PushParamRef(GetStrTarget());
		PushParam(ToModeLetters(changelist.getlist(), 450, beginit, lastit));
		PushModeParams();
	}