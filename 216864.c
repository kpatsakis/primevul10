	CmdResult Handle(User* user, const Params& parameters) CXX11_OVERRIDE
	{
		size_t origin = parameters.size() > 1 ? 1 : 0;
		if (parameters[origin].empty())
		{
			user->WriteNumeric(ERR_NOORIGIN, "No origin specified");
			return CMD_FAILURE;
		}

		// set the user as alive so they survive to next ping
		LocalUser* localuser = IS_LOCAL(user);
		if (localuser)
		{
			// Increase penalty unless we've sent a PING and this is the reply
			if (localuser->lastping)
				localuser->CommandFloodPenalty += 1000;
			else
				localuser->lastping = 1;
		}
		return CMD_SUCCESS;
	}