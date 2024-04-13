	static std::string ToModeLetters(const Modes::ChangeList::List& list, std::string::size_type maxlinelen, Modes::ChangeList::List::const_iterator beginit, Modes::ChangeList::List::const_iterator& lastit)
	{
		std::string ret;
		std::string::size_type paramlength = 0;
		char output_pm = '\0'; // current output state, '+' or '-'

		Modes::ChangeList::List::const_iterator i;
		for (i = beginit; i != list.end(); ++i)
		{
			const Modes::Change& item = *i;

			const char needed_pm = (item.adding ? '+' : '-');
			if (needed_pm != output_pm)
			{
				output_pm = needed_pm;
				ret.push_back(output_pm);
			}

			if (!item.param.empty())
				paramlength += item.param.length() + 1;
			if (ret.length() + 1 + paramlength > maxlinelen)
			{
				// Mode sequence is getting too long
				const char c = *ret.rbegin();
				if ((c == '+') || (c == '-'))
					ret.erase(ret.size()-1);
				break;
			}

			ret.push_back(item.mh->GetModeChar());
		}

		lastit = i;
		return ret;
	}