R_API int r_str_bits(char *strout, const ut8 *buf, int len, const char *bitz) {
	int i, j, idx;
	if (bitz) {
		for (i = j = 0; i<len && (!bitz||bitz[i]); i++) {
			if (i > 0 && (i % 8) == 0) {
				buf++;
			}
			if (*buf & (1 << (i % 8))) {
				strout[j++] = toupper ((const ut8)bitz[i]);
			}
		}
	} else {
		for (i = j = 0; i < len; i++) {
			idx = (i / 8);
			int bit = 7 - (i % 8);
			strout[j++] = (buf[idx] & (1 << bit))? '1' : '0';
		}
	}
	strout[j] = 0;
	return j;
}