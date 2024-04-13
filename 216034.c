R_API void r_str_range_foreach(const char *r, RStrRangeCallback cb, void *u) {
	const char *p = r;
	for (; *r; r++) {
		if (*r == ',') {
			cb (u, atoi (p));
			p = r + 1;
		}
		if (*r == '-') {
			if (p != r) {
				int from = atoi (p);
				int to = atoi (r + 1);
				for (; from <= to; from++) {
					cb (u, from);
				}
			} else {
				fprintf (stderr, "Invalid range\n");
			}
			for (r++; *r && *r != ',' && *r != '-'; r++) {
				;
			}
			p = r;
		}
	}
	if (*p) {
		cb (u, atoi (p));
	}
}