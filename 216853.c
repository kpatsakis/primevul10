	void PushTargetChan(char status, const Channel* targetchan)
	{
		if (status)
		{
			std::string rawtarget(1, status);
			rawtarget.append(targetchan->name);
			PushParam(rawtarget);
		}
		else
		{
			PushParamRef(targetchan->name);
		}
	}