	void ReadConfig(ConfigStatus& status) CXX11_OVERRIDE
	{
		cmdpart.msgwrap.ReadConfig("prefixpart", "suffixpart", "fixedpart");
		cmdquit.msgwrap.ReadConfig("prefixquit", "suffixquit", "fixedquit");
	}