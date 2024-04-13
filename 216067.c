static int strncmp_skip_color_codes(const char *s1, const char *s2, int n) {
	int i = 0, j = 0;
	int count = 0;
	for (i = 0, j = 0; s1[i]  && s2[j] && count < n; i++, j++, count++) {
		while (s1[i] == 0x1b) {
			while (s1[i] && s1[i] != 'm') {
				i++;
			}
			if (s1[i]) {
				i++;
			}
		}
		while (s2[j] == 0x1b) {
			while (s2[j] && s2[j] != 'm') {
				j++;
			}
			if (s2[j]) {
				j++;
			}
		}
		if (s1[i] != s2[j]) {
			return -1;
		}
	}

	if (count < n && s1[i] != s2[j]) {
		return -1;
	}

	return 0;
}