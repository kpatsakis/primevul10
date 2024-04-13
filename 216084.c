R_API char* r_str_replace(char *str, const char *key, const char *val, int g) {
	if (g == 'i') {
		return r_str_replace_icase (str, key, val, g, true);
	}
	r_return_val_if_fail (str && key && val, NULL);

	int off, i, slen;
	char *newstr, *p = str;
	int klen = strlen (key);
	int vlen = strlen (val);
	if (klen == 1 && vlen < 2) {
		r_str_replace_char (str, *key, *val);
		return str;
	}
	if (klen == vlen && !strcmp (key, val)) {
		return str;
	}
	slen = strlen (str);
	char *q = str;
	for (;;) {
		p = strstr (q, key);
		if (!p) {
			break;
		}
		off = (int)(size_t)(p - str);
		if (vlen != klen) {
			int tlen = slen - (off + klen);
			slen += vlen - klen;
			if (vlen > klen) {
				newstr = realloc (str, slen + 1);
				if (!newstr) {
					eprintf ("realloc fail\n");
					R_FREE (str);
					break;
				}
				str = newstr;
			}
			p = str + off;
			memmove (p + vlen, p + klen, tlen + 1);
		}
		memcpy (p, val, vlen);
		i = off + vlen;
		q = str + i;
		if (!g) {
			break;
		}
	}
	return str;
}