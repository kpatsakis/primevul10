	void InitFromNumeric(const ::Numeric::Numeric& numeric)
	{
		InitCommand(numeric.GetNumeric());
		for (std::vector<std::string>::const_iterator i = numeric.GetParams().begin(); i != numeric.GetParams().end(); ++i)
			PushParamRef(*i);
	}