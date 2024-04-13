	CommandPass(Module* parent)
		: SplitCommand(parent, "PASS", 1, 1)
	{
		works_before_reg = true;
		Penalty = 0;
		syntax = "<password>";
	}