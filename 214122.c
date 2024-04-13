static uint64_t time_win_to_unix(uint64_t win_time) {
	const size_t ns_in_sec = 10000000;
	const uint64_t sec_to_unix = 11644473600LL;
	return win_time / ns_in_sec - sec_to_unix;
}