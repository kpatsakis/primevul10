	void SetParams(Channel* Chantarget, User* Usertarget, const Modes::ChangeList& changelist)
	{
		ClearParams();

		chantarget = Chantarget;
		usertarget = Usertarget;
		beginit = changelist.getlist().begin();

		PushParamRef(GetStrTarget());
		PushParam(ToModeLetters(changelist.getlist(), 450, beginit, lastit));
		PushModeParams();
	}