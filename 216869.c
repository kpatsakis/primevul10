	void InitCommand(unsigned int number)
	{
		snprintf(numericstr, sizeof(numericstr), "%03u", number);
		SetCommand(numericstr);
	}