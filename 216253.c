R_API void r_str_stripLine(char *str, const char *key) {
	size_t i, j, klen, slen, off;
	const char *ptr;

	if (!str || !key) {
		return;
	}
	klen = strlen (key);
	slen = strlen (str);

	for (i = 0; i < slen; ) {
		ptr = (char*) r_mem_mem ((ut8*) str + i, slen - i, (ut8*) "\n", 1);
		if (!ptr) {
			ptr = (char*) r_mem_mem ((ut8*) str + i, slen - i, (ut8*) key, klen);
			if (ptr) {
				str[i] = '\0';
				break;
			}
			break;
		}

		off = (size_t) (ptr - (str + i)) + 1;

		ptr = (char*) r_mem_mem ((ut8*) str + i, off, (ut8*) key, klen);
		if (ptr) {
			for (j = i; j < slen - off + 1; j++) {
				str[j] = str[j + off];
			}
			slen -= off;
		} else {
			i += off;
		}
	}
}