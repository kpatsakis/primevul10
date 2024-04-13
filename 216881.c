	Version GetVersion() CXX11_OVERRIDE
	{
		return Version("Provides the AWAY, ISON, NICK, PART, PASS, PING, PONG, QUIT, USERHOST, and USER commands", VF_VENDOR|VF_CORE);
	}