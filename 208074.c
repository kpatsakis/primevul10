	pascal_string lazy_entry::dict_find_pstr(char const* name) const
	{
		lazy_entry const* e = dict_find(name);
		if (e == 0 || e->type() != lazy_entry::string_t) return pascal_string(0, 0);
		return e->string_pstr();
	}