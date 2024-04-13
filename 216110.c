R_API char *r_str_word_get0set(char *stra, int stralen, int idx, const char *newstr, int *newlen) {
	char *p = NULL;
	char *out;
	int alen, blen, nlen;
	if (!stra && !newstr) {
		return NULL;
	}
	if (stra) {
		p = (char *)r_str_word_get0 (stra, idx);
	}
	if (!p) {
		int nslen = strlen (newstr);
		out = malloc (nslen + 1);
		if (!out) {
			return NULL;
		}
		strcpy (out, newstr);
		out[nslen] = 0;
		if (newlen) {
			*newlen = nslen;
		}
		return out;
	}
	alen = (size_t)(p - stra);
	blen = stralen - ((alen + strlen (p)) + 1);
	if (blen < 0) {
		blen = 0;
	}
	nlen = alen + blen + strlen (newstr);
	out = malloc (nlen + 2);
	if (!out) {
		return NULL;
	}
	if (alen > 0) {
		memcpy (out, stra, alen);
	}
	memcpy (out + alen, newstr, strlen (newstr) + 1);
	if (blen > 0) {
		memcpy (out + alen + strlen (newstr) + 1, p + strlen (p) + 1, blen + 1);
	}
	out[nlen + 1] = 0;
	if (newlen) {
		*newlen = nlen + ((blen == 0)? 1 : 0);
	}
	return out;
}