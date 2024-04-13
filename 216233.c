R_API int r_str_utf16_to_utf8(ut8 *dst, int len_dst, const ut8 *src, int len_src, int little_endian) {
	ut8 *outstart = dst;
	ut8 *outend = dst + len_dst;
	ut16 *in = (ut16*)src;
	ut16 *inend;
	ut32 c, d, inlen;
	ut8 *tmp;
	int bits;

	if ((len_src % 2) == 1) {
		len_src--;
	}
	inlen = len_src / 2;
	inend = in + inlen;
	while ((in < inend) && (dst - outstart + 5 < len_dst)) {
		if (little_endian) {
			c = *in++;
		} else {
			tmp = (ut8*) in;
			c = *tmp++;
			if (!c && !*tmp) {
				break;
			}
			c = c | (((ut32)*tmp) << 8);
			in++;
		}
		if ((c & 0xFC00) == 0xD800) {    /* surrogates */
			if (in >= inend) {           /* (in > inend) shouldn't happens */
				break;
			}
			if (little_endian) {
				d = *in++;
			} else {
				tmp = (ut8*) in;
				d = *tmp++;
				d = d | (((ut32)*tmp) << 8);
				in++;
			}
			if ((d & 0xFC00) == 0xDC00) {
				c &= 0x03FF;
				c <<= 10;
				c |= d & 0x03FF;
				c += 0x10000;
			} else {
				return -2;
			}
		}

		/* assertion: c is a single UTF-4 value */
		if (dst >= outend) {
			break;
		}
		if (c < 0x80) {
			*dst++ =  c; bits= -6;
		} else if (c < 0x800) {
			*dst++ = ((c >> 6) & 0x1F) | 0xC0;
			bits =  0;
		} else if (c < 0x10000) {
			*dst++ = ((c >> 12) & 0x0F) | 0xE0;
			bits =  6;
		} else {
			*dst++ = ((c >> 18) & 0x07) | 0xF0;
			bits = 12;
		}

		for (; bits >= 0; bits -= 6) {
			if (dst >= outend) {
				break;
			}
			*dst++ = ((c >> bits) & 0x3F) | 0x80;
		}
	}
	len_dst = dst - outstart;
	return len_dst;
}