	static std::string ToModeLetters(const Modes::ChangeList& changelist)
	{
		// TODO: This assumes that std::string::max_size() >= UINT_MAX
		Modes::ChangeList::List::const_iterator dummy;
		return ToModeLetters(changelist.getlist(), UINT_MAX, changelist.getlist().begin(), dummy);
	}