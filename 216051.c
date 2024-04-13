static char *r_str_escape_(const char *buf, int dot_nl, bool parse_esc_seq, bool ign_esc_seq, bool show_asciidot, bool esc_bslash) {
	r_return_val_if_fail (buf, NULL);

	/* Worst case scenario, we convert every byte to a single-char escape
	 * (e.g. \n) if show_asciidot, or \xhh if !show_asciidot */
	char *new_buf = malloc (1 + strlen (buf) * (show_asciidot ? 2 : 4));
	if (!new_buf) {
		return NULL;
	}
	const char *p = buf;
	char *q = new_buf;
	while (*p) {
		switch (*p) {
		case 0x1b: // ESC
			if (parse_esc_seq) {
				const char *start_seq = p;
				p++;
				/* Parse the ANSI code (only the graphic mode
				 * set ones are supported) */
				if (*p == '\0') {
					goto out;
				}
				if (*p == '[') {
					for (p++; *p != 'm'; p++) {
						if (*p == '\0') {
							goto out;
						}
					}
					if (!ign_esc_seq) {
						memcpy (q, start_seq, p - start_seq + 1);
						q += (p - start_seq + 1);
					}
				}
				break;
			}
			/* fallthrough */
		default:
			r_str_byte_escape (p, &q, dot_nl, show_asciidot, esc_bslash);
		}
		p++;
	}
out:
	*q = '\0';
	return new_buf;
}