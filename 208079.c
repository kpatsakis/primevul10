	lazy_entry* lazy_entry::dict_append(char const* name)
	{
		TORRENT_ASSERT(m_type == dict_t);
		TORRENT_ASSERT(m_size <= m_capacity);
		if (m_capacity == 0)
		{
			int capacity = lazy_entry_dict_init;
			m_data.dict = new (std::nothrow) lazy_dict_entry[capacity];
			if (m_data.dict == 0) return 0;
			m_capacity = capacity;
		}
		else if (m_size == m_capacity)
		{
			int capacity = m_capacity * lazy_entry_grow_factor / 100;
			lazy_dict_entry* tmp = new (std::nothrow) lazy_dict_entry[capacity];
			if (tmp == 0) return 0;
			std::memcpy(tmp, m_data.dict, sizeof(lazy_dict_entry) * m_size);
			for (int i = 0; i < int(m_size); ++i) m_data.dict[i].val.release();
			delete[] m_data.dict;
			m_data.dict = tmp;
			m_capacity = capacity;
		}

		TORRENT_ASSERT(m_size < m_capacity);
		lazy_dict_entry& ret = m_data.dict[m_size++];
		ret.name = name;
		return &ret.val;
	}