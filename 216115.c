R_API const char *r_str_pad(const char ch, int sz) {
	static char pad[1024];
	if (sz < 0) {
		sz = 0;
	}
	memset (pad, ch, R_MIN (sz, sizeof (pad)));
	if (sz < sizeof (pad)) {
		pad[sz] = 0;
	}
	pad[sizeof(pad) - 1] = 0;
	return pad;
}