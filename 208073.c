	void lazy_entry::construct_string(char const* start, int length)
	{
		TORRENT_ASSERT(m_type == none_t);
		m_type = string_t;
		m_data.start = start;
		m_size = length;
		m_begin = start - 1 - num_digits(length);
		m_len = start - m_begin + length;
	}