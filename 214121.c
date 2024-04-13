static int parse_htime_item(struct archive_read* a, char unix_time,
    uint64_t* where, ssize_t* extra_data_size)
{
	if(unix_time) {
		uint32_t time_val;
		if(!read_u32(a, &time_val))
			return ARCHIVE_EOF;

		*extra_data_size -= 4;
		*where = (uint64_t) time_val;
	} else {
		uint64_t windows_time;
		if(!read_u64(a, &windows_time))
			return ARCHIVE_EOF;

		*where = time_win_to_unix(windows_time);
		*extra_data_size -= 8;
	}

	return ARCHIVE_OK;
}