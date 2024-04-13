	void lazy_entry::clear()
	{
		switch (m_type)
		{
			case list_t: delete[] m_data.list; break;
			case dict_t: delete[] m_data.dict; break;
			default: break;
		}
		m_data.start = 0;
		m_size = 0;
		m_capacity = 0;
		m_type = none_t;
	}