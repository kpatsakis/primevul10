	char const* parse_int(char const* start, char const* end, char delimiter
		, boost::int64_t& val, bdecode_errors::error_code_enum& ec)
	{
		while (start < end && *start != delimiter)
		{
			if (!numeric(*start))
			{
				ec = bdecode_errors::expected_string;
				return start;
			}
			if (val > INT64_MAX / 10)
			{
				ec = bdecode_errors::overflow;
				return start;
			}
			val *= 10;
			int digit = *start - '0';
			if (val > INT64_MAX - digit)
			{
				ec = bdecode_errors::overflow;
				return start;
			}
			val += digit;
			++start;
		}
		if (*start != delimiter)
			ec = bdecode_errors::expected_colon;
		return start;
	}