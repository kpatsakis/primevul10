R_API const char *r_str_lastbut(const char *s, char ch, const char *but) {
	int idx, _b = 0;
	ut8 *b = (ut8*)&_b;
	const char *isbut, *p, *lp = NULL;
	const int bsz = sizeof (_b) * 8;
	if (!but) {
		return r_str_lchr (s, ch);
	}
	if (strlen (but) >= bsz) {
		eprintf ("r_str_lastbut: but string too long\n");
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
			lp = p;
		}
	}
	return lp;
}