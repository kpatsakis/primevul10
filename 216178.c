R_API char *r_str_replace_icase(char *str, const char *key, const char *val, int g, int keep_case) {
	r_return_val_if_fail (str && key && val, NULL);

	int off, i, klen, vlen, slen;
	char *newstr, *p = str;
	klen = strlen (key);
	vlen = strlen (val);

	slen = strlen (str);
	for (i = 0; i < slen;) {
		p = (char *)r_str_casestr (str + i, key);
		if (!p) {
			break;
		}
		off = (int)(size_t) (p - str);
		if (vlen != klen) {
			int tlen = slen - (off + klen);
			slen += vlen - klen;
			if (vlen > klen) {
				newstr = realloc (str, slen + 1);
				if (!newstr) {
					goto alloc_fail;
				}
				str = newstr;
			}
			p = str + off;
			memmove (p + vlen, p + klen, tlen + 1);
		}

		if (keep_case) {
			char *tmp_val = strdup (val);
			char *str_case = r_str_ndup (p, klen);
			if (!tmp_val || !str_case) {
				free (tmp_val);
				free (str_case);
				goto alloc_fail;
			}
			tmp_val = r_str_replace_icase (tmp_val, key, str_case, 0, 0);
			free (str_case);
			if (!tmp_val) {
				goto alloc_fail;
			}
			memcpy (p, tmp_val, vlen);
			free (tmp_val);
		} else {
			memcpy (p, val, vlen);
		}

		i = off + vlen;
		if (!g) {
			break;
		}
	}
	return str;

alloc_fail:
	eprintf ("alloc fail\n");
	free (str);
	return NULL;
}