	boost::int64_t lazy_entry::int_value() const
	{
		TORRENT_ASSERT(m_type == int_t);
		boost::int64_t val = 0;
		bool negative = false;
		if (*m_data.start == '-') negative = true;
		bdecode_errors::error_code_enum ec = bdecode_errors::no_error;
		parse_int(m_data.start + negative
			, m_data.start + m_size, 'e', val, ec);
		if (ec) return 0;
		if (negative) val = -val;
		return val;
	}