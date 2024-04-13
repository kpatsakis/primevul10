R_API char *r_str_escape_utf8_for_json(const char *buf, int buf_size) {
	char *new_buf, *q;
	const char *p, *end;
	RRune ch;
	int i, len, ch_bytes;

	if (!buf) {
		return NULL;
	}
	len = buf_size < 0 ? strlen (buf) : buf_size;
	end = buf + len;
	/* Worst case scenario, we convert every byte to \u00hh */
	new_buf = malloc (1 + (len * 6));
	if (!new_buf) {
		return NULL;
	}
	p = buf;
	q = new_buf;
	while (p < end) {
		ch_bytes = r_utf8_decode ((ut8 *)p, end - p, &ch);
		if (ch_bytes == 1) {
			switch (*p) {
			case '\n':
				*q++ = '\\';
				*q++ = 'n';
				break;
			case '\r':
				*q++ = '\\';
				*q++ = 'r';
				break;
			case '\\':
				*q++ = '\\';
				*q++ = '\\';
				break;
#if 0
			case '/': /* has 2-char esc seq in JSON spec, but escaping is optional */
				*q++ = '\\';
				*q++ = '/';
				break;
#endif
			case '\t':
				*q++ = '\\';
				*q++ = 't';
				break;
			case '"' :
				*q++ = '\\';
				*q++ = '"';
				break;
			case '\f':
				*q++ = '\\';
				*q++ = 'f';
				break;
			case '\b':
				*q++ = '\\';
				*q++ = 'b';
				break;
			default:
				if (!IS_PRINTABLE (*p)) {
					*q++ = '\\';
					*q++ = 'u';
					*q++ = '0';
					*q++ = '0';
					*q++ = "0123456789abcdef"[*p >> 4 & 0xf];
					*q++ = "0123456789abcdef"[*p & 0xf];
				} else {
					*q++ = *p;
				}
			}
		} else if (ch_bytes == 4) {
			if (r_isprint (ch)) {
				// Assumes buf is UTF8-encoded
				for (i = 0; i < ch_bytes; i++) {
					*q++ = *(p + i);
				}
			} else {
				RRune high, low;
				ch -= 0x10000;
				high = 0xd800 + (ch >> 10 & 0x3ff);
				low = 0xdc00 + (ch & 0x3ff);
				*q++ = '\\';
				*q++ = 'u';
				for (i = 2; i >= 0; i -= 2) {
					*q++ = "0123456789abcdef"[high >> 4 * (i + 1) & 0xf];
					*q++ = "0123456789abcdef"[high >> 4 * i & 0xf];
				}
				*q++ = '\\';
				*q++ = 'u';
				for (i = 2; i >= 0; i -= 2) {
					*q++ = "0123456789abcdef"[low >> 4 * (i + 1) & 0xf];
					*q++ = "0123456789abcdef"[low >> 4 * i & 0xf];
				}
			}
		} else if (ch_bytes > 1) {
			if (r_isprint (ch)) {
				// Assumes buf is UTF8-encoded
				for (i = 0; i < ch_bytes; i++) {
					*q++ = *(p + i);
				}
			} else {
				*q++ = '\\';
				*q++ = 'u';
				for (i = 2; i >= 0; i -= 2) {
					*q++ = "0123456789abcdef"[ch >> 4 * (i + 1) & 0xf];
					*q++ = "0123456789abcdef"[ch >> 4 * i & 0xf];
				}
			}
		} else { // ch_bytes == 0
			// Outside JSON spec, but apparently no better
			// alternative if need to reconstruct the original string
			*q++ = '\\';
			*q++ = 'x';
			*q++ = "0123456789abcdef"[*p >> 4 & 0xf];
			*q++ = "0123456789abcdef"[*p & 0xf];
			ch_bytes = 1;
		}
		p += ch_bytes;
	}
	*q = '\0';
	return new_buf;
}