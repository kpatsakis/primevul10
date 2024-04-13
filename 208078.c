	pascal_string lazy_entry::list_pstr_at(int i) const
	{
		lazy_entry const* e = list_at(i);
		if (e == 0 || e->type() != lazy_entry::string_t) return pascal_string(0, 0);
		return e->string_pstr();
	}