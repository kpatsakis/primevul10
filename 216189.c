static char *r_str_escape_utf(const char *buf, int buf_size, RStrEnc enc, bool show_asciidot, bool esc_bslash, bool keep_printable) {
	char *new_buf, *q;
	const char *p, *end;
	RRune ch;
	int i, len, ch_bytes;

	if (!buf) {
		return NULL;
	}
	switch (enc) {
	case R_STRING_ENC_UTF16LE:
	case R_STRING_ENC_UTF16BE:
	case R_STRING_ENC_UTF32LE:
	case R_STRING_ENC_UTF32BE:
		if (buf_size < 0) {
			return NULL;
		}
		if (enc == R_STRING_ENC_UTF16LE || enc == R_STRING_ENC_UTF16BE) {
			end = (char *)r_mem_mem_aligned ((ut8 *)buf, buf_size, (ut8 *)"\0\0", 2, 2);
		} else {
			end = (char *)r_mem_mem_aligned ((ut8 *)buf, buf_size, (ut8 *)"\0\0\0\0", 4, 4);
		}
		if (!end) {
			end = buf + buf_size - 1; /* TODO: handle overlong strings properly */
		}
		len = end - buf;
		break;
	default:
		len = strlen (buf);
		end = buf + len;
	}
	/* Worst case scenario, we convert every byte to \xhh */
	new_buf = malloc (1 + (len * 4));
	if (!new_buf) {
		return NULL;
	}
	p = buf;
	q = new_buf;
	while (p < end) {
		switch (enc) {
		case R_STRING_ENC_UTF16LE:
		case R_STRING_ENC_UTF16BE:
		case R_STRING_ENC_UTF32LE:
		case R_STRING_ENC_UTF32BE:
			if (enc == R_STRING_ENC_UTF16LE || enc == R_STRING_ENC_UTF16BE) {
				ch_bytes = r_utf16_decode ((ut8 *)p, end - p, &ch, enc == R_STRING_ENC_UTF16BE);
			} else {
				ch_bytes = r_utf32_decode ((ut8 *)p, end - p, &ch, enc == R_STRING_ENC_UTF32BE);
			}
			if (ch_bytes == 0) {
				p++;
				continue;
			}
			break;
		default:
			ch_bytes = r_utf8_decode ((ut8 *)p, end - p, &ch);
			if (ch_bytes == 0) {
				ch_bytes = 1;
			}
		}
		if (show_asciidot && !IS_PRINTABLE(ch)) {
			*q++ = '.';
		} else if (ch_bytes > 1) {
			if (keep_printable) {
				q += r_utf8_encode ((ut8 *)q, ch);
			} else {
				*q++ = '\\';
				*q++ = ch_bytes == 4 ? 'U' : 'u';
				for (i = ch_bytes == 4 ? 6 : 2; i >= 0; i -= 2) {
					*q++ = "0123456789abcdef"[ch >> 4 * (i + 1) & 0xf];
					*q++ = "0123456789abcdef"[ch >> 4 * i & 0xf];
				}
			}
		} else {
			int offset = enc == R_STRING_ENC_UTF16BE ? 1 : enc == R_STRING_ENC_UTF32BE ? 3 : 0;
			r_str_byte_escape (p + offset, &q, false, false, esc_bslash);
		}
		switch (enc) {
		case R_STRING_ENC_UTF16LE:
		case R_STRING_ENC_UTF16BE:
			p += ch_bytes < 2 ? 2 : ch_bytes;
			break;
		case R_STRING_ENC_UTF32LE:
		case R_STRING_ENC_UTF32BE:
			p += 4;
			break;
		default:
			p += ch_bytes;
		}
	}
	*q = '\0';
	return new_buf;
}