R_API int r_str_unescape(char *buf) {
	unsigned char ch = 0, ch2 = 0;
	int err = 0;
	int i;

	for (i = 0; buf[i]; i++) {
		if (buf[i] != '\\') {
			continue;
		}
		int esc_seq_len = 2;
		switch (buf[i + 1]) {
		case 'e':
			buf[i] = 0x1b;
			break;
		case '\\':
			buf[i] = '\\';
			break;
		case 'r':
			buf[i] = 0x0d;
			break;
		case 'n':
			buf[i] = 0x0a;
			break;
		case 'a':
			buf[i] = 0x07;
			break;
		case 'b':
			buf[i] = 0x08;
			break;
		case 't':
			buf[i] = 0x09;
			break;
		case 'v':
			buf[i] = 0x0b;
			break;
		case 'f':
			buf[i] = 0x0c;
			break;
		case 'x':
			err = ch2 = ch = 0;
			if (!buf[i + 2] || !buf[i + 3]) {
				eprintf ("Unexpected end of string.\n");
				return 0;
			}
			err |= r_hex_to_byte (&ch,  buf[i + 2]);
			err |= r_hex_to_byte (&ch2, buf[i + 3]);
			if (err) {
				eprintf ("Error: Non-hexadecimal chars in input.\n");
				return 0; // -1?
			}
			buf[i] = (ch << 4) + ch2;
			esc_seq_len = 4;
			break;
		default:
			if (IS_OCTAL (buf[i + 1])) {
				int num_digits = 1;
				buf[i] = buf[i + 1] - '0';
				if (IS_OCTAL (buf[i + 2])) {
					num_digits++;
					buf[i] = (ut8)buf[i] * 8 + (buf[i + 2] - '0');
					if (IS_OCTAL (buf[i + 3])) {
						num_digits++;
						buf[i] = (ut8)buf[i] * 8 + (buf[i + 3] - '0');
					}
				}
				esc_seq_len = 1 + num_digits;
			} else {
				eprintf ("Error: Unknown escape sequence.\n");
				return 0; // -1?
			}
		}
		memmove (buf + i + 1, buf + i + esc_seq_len, strlen (buf + i + esc_seq_len) + 1);
	}
	return i;
}