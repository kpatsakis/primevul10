	lazy_entry* lazy_entry::dict_find(std::string const& name)
	{
		TORRENT_ASSERT(m_type == dict_t);
		for (int i = 0; i < int(m_size); ++i)
		{
			lazy_dict_entry& e = m_data.dict[i];
			if (name.size() != e.val.m_begin - e.name) continue;
			if (std::equal(name.begin(), name.end(), e.name))
				return &e.val;
		}
		return 0;
	}