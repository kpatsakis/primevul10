R_API const char *r_str_firstbut(const char *s, char ch, const char *but) {
	int idx, _b = 0;
	ut8 *b = (ut8*)&_b;
	const char *isbut, *p;
	const int bsz = sizeof (_b) * 8;
	if (!but) {
		return strchr (s, ch);
	}
	if (strlen (but) >= bsz) {
		eprintf ("r_str_firstbut: but string too long\n");
		return NULL;
	}
	for (p = s; *p; p++) {
		isbut = strchr (but, *p);
		if (isbut) {
			idx = (int)(size_t)(isbut - but);
			_b = R_BIT_TOGGLE (b, idx);
			continue;
		}
		if (*p == ch && !_b) {
			return p;
		}
	}
	return NULL;
}