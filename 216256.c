R_API int r_str_ansi_filter(char *str, char **out, int **cposs, int len) {
	int i, j, *cps;

	if (len == 0) {
		return 0;
	}
	if (len < 0) {
		len = strlen (str);
	}
	char *tmp = malloc (len + 1);
	if (!tmp) {
		return -1;
	}
	memcpy (tmp, str, len + 1);
	cps = calloc (len + 1, sizeof (int));
	if (!cps) {
		free (tmp);
		return -1;
	}

	for (i = j = 0; i < len; i++) {
		if (tmp[i] == 0x1b) {
			size_t chlen = __str_ansi_length (str + i);
			if (chlen > 1) {
				i += chlen;
				i--;
			}
		} else {
			str[j] = tmp[i];
			cps[j] = i;
			j++;
		}
	}
	str[j] = tmp[i];

	if (out) {
		*out = tmp;
	} else {
		free (tmp);
	}

	if (cposs) {
		*cposs = cps;
	} else {
		free (cps);
	}

	return j;
}