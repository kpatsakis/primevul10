		int fail(int* error_pos
			, std::vector<lazy_entry*>& stack
			, char const* start
			, char const* orig_start)
		{
			while (!stack.empty()) {
				lazy_entry* top = stack.back();
				if (top->type() == lazy_entry::dict_t || top->type() == lazy_entry::list_t)
				{
					top->pop();
					break;
				}
				stack.pop_back();
			}
			if (error_pos) *error_pos = start - orig_start;
			return -1;
		}