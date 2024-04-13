R_API void r_core_set_asmqjmps(RCore *core, char *str, size_t len, int pos) {
	if (core->is_asmqjmps_letter) {
		int i, j = 0;
		// if (pos > 0) {
			pos --;
		////  }
		for (i = 0; i < R_CORE_ASMQJMPS_LEN_LETTERS - 1; i++) {
			int div = pos / letter_divs[i];
			pos %= letter_divs[i];
			if (div > 0 && j < len) {
				str[j++] = 'A' + div - 1;
			}
		}
		if (j < len) {
			int div = pos % R_CORE_ASMQJMPS_LETTERS;
			str[j++] = 'a' + div;
		}
		str[j] = '\0';
	} else {
		snprintf (str, len, "%d", pos);
	}
}