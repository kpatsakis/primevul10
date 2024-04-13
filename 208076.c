	boost::int64_t lazy_entry::dict_find_int_value(char const* name, boost::int64_t default_val) const
	{
		lazy_entry const* e = dict_find(name);
		if (e == 0 || e->type() != lazy_entry::int_t) return default_val;
		return e->int_value();
	}