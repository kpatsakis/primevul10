	std::string print_entry(lazy_entry const& e, bool single_line, int indent)
	{
		char indent_str[200];
		memset(indent_str, ' ', 200);
		indent_str[0] = ',';
		indent_str[1] = '\n';
		indent_str[199] = 0;
		if (indent < 197 && indent >= 0) indent_str[indent+2] = 0;
		std::string ret;
		switch (e.type())
		{
			case lazy_entry::none_t: return "none";
			case lazy_entry::int_t:
			{
				char str[100];
				snprintf(str, sizeof(str), "%" PRId64, e.int_value());
				return str;
			}
			case lazy_entry::string_t:
			{
				bool printable = true;
				char const* str = e.string_ptr();
				for (int i = 0; i < e.string_length(); ++i)
				{
					char c = str[i];
					if (c >= 32 && c < 127) continue;
					printable = false;
					break;
				}
				ret += "'";
				if (printable)
				{
					if (single_line && e.string_length() > 30)
					{
						ret.append(e.string_ptr(), 14);
						ret += "...";
						ret.append(e.string_ptr() + e.string_length()-14, 14);
					}
					else
						ret.append(e.string_ptr(), e.string_length());
					ret += "'";
					return ret;
				}
				if (single_line && e.string_length() > 20)
				{
					for (int i = 0; i < 9; ++i)
					{
						char tmp[5];
						snprintf(tmp, sizeof(tmp), "%02x", (unsigned char)str[i]);
						ret += tmp;
					}
					ret += "...";
					for (int i = e.string_length() - 9
						, len(e.string_length()); i < len; ++i)
					{
						char tmp[5];
						snprintf(tmp, sizeof(tmp), "%02x", (unsigned char)str[i]);
						ret += tmp;
					}
				}
				else
				{
					for (int i = 0; i < e.string_length(); ++i)
					{
						char tmp[5];
						snprintf(tmp, sizeof(tmp), "%02x", (unsigned char)str[i]);
						ret += tmp;
					}
				}
				ret += "'";
				return ret;
			}
			case lazy_entry::list_t:
			{
				ret += '[';
				bool one_liner = line_longer_than(e, 200) != -1 || single_line;

				if (!one_liner) ret += indent_str + 1;
				for (int i = 0; i < e.list_size(); ++i)
				{
					if (i == 0 && one_liner) ret += " ";
					ret += print_entry(*e.list_at(i), single_line, indent + 2);
					if (i < e.list_size() - 1) ret += (one_liner?", ":indent_str);
					else ret += (one_liner?" ":indent_str+1);
				}
				ret += "]";
				return ret;
			}
			case lazy_entry::dict_t:
			{
				ret += "{";
				bool one_liner = line_longer_than(e, 200) != -1 || single_line;

				if (!one_liner) ret += indent_str+1;
				for (int i = 0; i < e.dict_size(); ++i)
				{
					if (i == 0 && one_liner) ret += " ";
					std::pair<std::string, lazy_entry const*> ent = e.dict_at(i);
					ret += "'";
					ret += ent.first;
					ret += "': ";
					ret += print_entry(*ent.second, single_line, indent + 2);
					if (i < e.dict_size() - 1) ret += (one_liner?", ":indent_str);
					else ret += (one_liner?" ":indent_str+1);
				}
				ret += "}";
				return ret;
			}
		}
		return ret;
	}