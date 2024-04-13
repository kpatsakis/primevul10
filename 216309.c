R_API char* r_str_highlight(char *str, const char *word, const char *color, const char *color_reset) {
	if (!str || !*str) {
		return NULL;
	}
	ut32 i = 0, j = 0, to_copy;
	char *start = str;
	ut32 l_str = strlen (str);
	ut32 l_reset = strlen (color_reset);
	ut32 l_color = color? strlen (color): 0;
	if (!color) {
		return strdup (str);
	}
	if (!word || !*word) {
		return r_str_newf ("%s%s%s", color, str, color_reset);
	}
	ut32 l_word = strlen (word);
	// XXX don't use static buffers
	char o[1024] = {0};
	while (start && (start < str + l_str)) {
		int copied = 0;
		// find first letter
		start = strchr_skip_color_codes (str + i, *word);
		if (start) {
			to_copy = start - (str + i);
			if (to_copy + j + 1 > sizeof (o)) {
				// XXX. no limits
				break;
			}
			strncpy (o + j, str + i, to_copy);
			i += to_copy;
			j += to_copy;
			if (!strncmp_skip_color_codes (start, word, l_word)) {
				if (j + strlen (color) >= sizeof (o)) {
					// XXX. no limits
					break;
				}
				strcpy (o + j, color);
				j += l_color;
				if (j + l_word >= sizeof (o)) {
					// XXX. no limits
					break;
				}
				copied = strncpy_with_color_codes (o + j, str + i, l_word);
				i += copied;
				j += copied;
				if (j + strlen (color_reset) >= sizeof (o)) {
					// XXX. no limits
					break;
				}
				strcpy (o + j, color_reset);
				j += l_reset;
			} else {
				o[j++] = str[i++];
			}
		} else {
			if (j + strlen (str + i) >= sizeof (o)) {
				break;
			}
			strcpy (o + j, str + i);
			break;
		}
	}
	return strdup (o);
}