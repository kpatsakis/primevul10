	boost::int64_t lazy_entry::list_int_value_at(int i, boost::int64_t default_val) const
	{
		lazy_entry const* e = list_at(i);
		if (e == 0 || e->type() != lazy_entry::int_t) return default_val;
		return e->int_value();
	}