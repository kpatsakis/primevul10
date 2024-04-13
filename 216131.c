R_API char* r_str_replace_thunked(char *str, char *clean, int *thunk, int clen,
				  const char *key, const char *val, int g) {
	int i, klen, vlen, slen, delta = 0, bias;
	char *newstr, *scnd, *p = clean, *str_p;

	if (!str || !key || !val || !clean || !thunk) {
		return NULL;
	}
	klen = strlen (key);
	vlen = strlen (val);
	if (klen == vlen && !strcmp (key, val)) {
		return str;
	}
	slen = strlen (str) + 1;

	for (i = 0; i < clen; ) {
		p = (char *)r_mem_mem (
			(const ut8*)clean + i, clen - i,
			(const ut8*)key, klen);
		if (!p) {
			break;
		}
		i = (int)(size_t)(p - clean);
		/* as the original string changes size during replacement
		 * we need delta to keep track of it*/
		str_p = str + thunk[i] + delta;

		int newo = thunk[i + klen] - thunk[i];
		r_str_ansi_filter (str_p, NULL, NULL, newo);
		scnd = strdup (str_p + newo);
		bias = vlen - newo;

		slen += bias;
		// HACK: this 32 avoids overwrites wtf
		newstr = realloc (str, slen + klen);
		if (!newstr) {
			eprintf ("realloc fail\n");
			R_FREE (str);
			free (scnd);
			break;
		}
		str = newstr;
		str_p = str + thunk[i] + delta;
		memcpy (str_p, val, vlen);
		memcpy (str_p + vlen, scnd, strlen (scnd) + 1);
		i += klen;
		delta += bias;
		free (scnd);
		if (!g) {
			break;
		}
	}
	return str;
}