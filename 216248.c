R_API ut64 r_str_bits_from_string(const char *buf, const char *bitz) {
	ut64 out = 0LL;
	/* return the numeric value associated to a string (rflags) */
	for (; *buf; buf++) {
		char *ch = strchr (bitz, toupper ((const unsigned char)*buf));
		if (!ch) {
			ch = strchr (bitz, tolower ((const unsigned char)*buf));
		}
		if (ch) {
			int bit = (int)(size_t)(ch - bitz);
			out |= (ut64)(1LL << bit);
		} else {
			return UT64_MAX;
		}
	}
	return out;
}