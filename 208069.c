	std::pair<char const*, int> lazy_entry::data_section() const
	{
		typedef std::pair<char const*, int> return_t;
		return return_t(m_begin, m_len);
	}