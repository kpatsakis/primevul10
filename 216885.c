	CommandPing(Module* parent)
		: SplitCommand(parent, "PING", 1)
	{
		syntax = "<cookie> [<servername>]";
	}