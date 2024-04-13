R_API char *r_str_ansi_crop(const char *str, ut32 x, ut32 y, ut32 x2, ut32 y2) {
	char *r, *r_end, *ret;
	const char *s, *s_start;
	size_t r_len, str_len = 0, nr_of_lines = 0;
	ut32 ch = 0, cw = 0;
	if (x2 <= x || y2 <= y || !str) {
		return strdup ("");
	}
	s = s_start = str;
	while (*s) {
		str_len++;
		if (*s == '\n') {
			nr_of_lines++;
		}
		s++;
	}
	r_len = str_len + nr_of_lines * strlen (Color_RESET) + 1;
	r = ret = malloc (r_len);
	if (!r) {
		return NULL;
	}
	r_end = r + r_len;
	while (*str) {
		/* crop height */
		if (ch >= y2) {
			r--;
			break;
		}
		if (*str == '\n') {
			if (ch >= y && ch < y2) {
				const char *reset = Color_RESET "\n";
				if (strlen (reset) < (r_end - r)) {
					const int reset_length = strlen (reset);
					memcpy (r, reset, reset_length + 1);
					r += reset_length;
				}
			}
			str++;
			ch++;
			cw = 0;
		} else {
			if (ch >= y && ch < y2) {
				if ((*str & 0xc0) == 0x80) {
					if (cw > x) {
						*r++ = *str++;
					} else {
						str++;
					}
					continue;
				}
				if (r_str_char_fullwidth (str, str_len - (str - s_start))) {
					cw++;
					if (cw == x) {
						*r++ = ' ';
						str++;
						continue;
					}
				}
				if (*str == 0x1b && *(str + 1) == '[') {
					const char *ptr = str;
					if ((r_end - r) > 2) {
						/* copy 0x1b and [ */
						*r++ = *str++;
						*r++ = *str++;
						for (ptr = str; *ptr && *ptr != 'J' && *ptr != 'm' && *ptr != 'H'; ptr++) {
							*r++ = *ptr;
						}
						*r++ = *ptr++;
					}
					str = ptr;
					continue;
				} else if (cw >= x && cw < x2) {
					*r++ = *str;
				}
			}
			/* skip until newline */
			if (cw >= x2) {
				while (*str && *str != '\n') {
					str++;
				}
			} else {
				str++;
			}
			cw++;
		}
	}
	*r = 0;
	return ret;
}