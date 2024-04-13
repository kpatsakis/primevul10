	void PushTargetUser(const User* targetuser)
	{
		if (targetuser->registered & REG_NICK)
			PushParamRef(targetuser->nick);
		else
			PushParam("*");
	}