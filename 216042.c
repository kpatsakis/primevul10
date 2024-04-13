R_API char *r_str_crop(const char *str, unsigned int x, unsigned int y,
		unsigned int x2, unsigned int y2) {
	char *r, *ret;
	unsigned int ch = 0, cw = 0;
	if (x2 < 1 || y2 < 1 || !str) {
		return strdup ("");
	}
	r = ret = strdup (str);
	while (*str) {
		/* crop height */
		if (ch >= y2) {
			r--;
			break;
		}

		if (*str == '\n') {
			if (ch >= y && ch < y2) {
				*r++ = *str;
			}
			str++;
			ch++;
			cw = 0;
		} else {
			if (ch >= y && ch < y2 && cw >= x && cw < x2) {
				*r++ = *str;
			}
			/* crop width */
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