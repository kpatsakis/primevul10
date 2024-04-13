	boost::system::error_category& get_bdecode_category()
	{
		static bdecode_error_category bdecode_category;
		return bdecode_category;
	}