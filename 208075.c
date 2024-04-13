	std::pair<std::string, lazy_entry const*> lazy_entry::dict_at(int i) const
	{
		TORRENT_ASSERT(m_type == dict_t);
		TORRENT_ASSERT(i < int(m_size));
		lazy_dict_entry const& e = m_data.dict[i];
		return std::make_pair(std::string(e.name, e.val.m_begin - e.name), &e.val);
	}