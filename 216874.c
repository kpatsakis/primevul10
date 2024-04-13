	CommandPong(Module* parent)
		: Command(parent, "PONG", 1)
	{
		Penalty = 0;
		syntax = "<cookie> [<servername>]";
	}