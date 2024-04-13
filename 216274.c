R_API void r_str_byte_escape(const char *p, char **dst, int dot_nl, bool default_dot, bool esc_bslash) {
	char *q = *dst;
	switch (*p) {
	case '\n':
		*q++ = '\\';
		*q++ = dot_nl ? 'l' : 'n';
		break;
	case '\r':
		*q++ = '\\';
		*q++ = 'r';
		break;
	case '\\':
		*q++ = '\\';
		if (esc_bslash) {
			*q++ = '\\';
		}
		break;
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
	case '\v':
		*q++ = '\\';
		*q++ = 'v';
		break;
	case '\a':
		*q++ = '\\';
		*q++ = 'a';
		break;
	default:
		/* Outside the ASCII printable range */
		if (!IS_PRINTABLE (*p)) {
			if (default_dot) {
				*q++ = '.';
			} else {
				*q++ = '\\';
				*q++ = 'x';
				*q++ = "0123456789abcdef"[*p >> 4 & 0xf];
				*q++ = "0123456789abcdef"[*p & 0xf];
			}
		} else {
			*q++ = *p;
		}
	}
	*dst = q;
}