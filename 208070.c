	lazy_entry const* lazy_entry::dict_find_int(char const* name) const
	{
		lazy_entry const* e = dict_find(name);
		if (e == 0 || e->type() != lazy_entry::int_t) return 0;
		return e;
	}