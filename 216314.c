R_API bool r_str_range_in(const char *r, ut64 addr) {
	const char *p = r;
	ut64 min = UT64_MAX;
	ut64 max = 0;
	if (!r) {
		return false;
	}
	for (; *r; r++) {
		if (*r == ',') {
			if (max == 0) {
				if (addr == r_num_get (NULL, p)) {
					return true;
				}
			} else {
				if (addr >= min && addr <= r_num_get (NULL, p)) {
					return true;
				}
			}
			p = r + 1;
		}
		if (*r == '-') {
			if (p != r) {
				ut64 from = r_num_get (NULL, p);
				ut64 to = r_num_get (NULL, r + 1);
				if (addr >= from && addr <= to) {
					return true;
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
		if (addr == r_num_get (NULL, p)) {
			return true;
		}
	}
	return false;
}