R_API char *r_str_format_msvc_argv(size_t argc, const char **argv) {
	RStrBuf sb;
	r_strbuf_init (&sb);

	size_t i;
	for (i = 0; i < argc; i++) {
		if (i > 0) {
			r_strbuf_append (&sb, " ");
		}
		const char *arg = argv[i];
		bool must_escape = strchr (arg, '\"') != NULL;
		bool must_quote = strpbrk (arg, " \t") != NULL || !*arg;
		if (!must_escape && must_quote && *arg && arg[strlen (arg) - 1] == '\\') {
			// if the last char is a bs and we would quote it, we must also escape
			must_escape = true;
		}
		if (must_quote) {
			r_strbuf_append (&sb, "\"");
		}
		if (must_escape) {
			size_t bs_count = 0; // bullshit counter
			for (; *arg; arg++) {
				switch (*arg) {
				case '\"':
					for (; bs_count; bs_count--) {
						// backslashes must be escaped iff they precede a "
						// so just duplicate the number of backslashes already printed
						r_strbuf_append (&sb, "\\");
					}
					r_strbuf_append (&sb, "\\\"");
					break;
				case '\\':
					bs_count++;
					r_strbuf_append (&sb, "\\");
					break;
				default:
					bs_count = 0;
					r_strbuf_append_n (&sb, arg, 1);
					break;
				}
			}
			if (must_quote) {
				// there will be a quote after this so we have to escape bs here as well
				for (; bs_count; bs_count--) {
					r_strbuf_append (&sb, "\\");
				}
			}
		} else {
			r_strbuf_append (&sb, arg);
		}
		if (must_quote) {
			r_strbuf_append (&sb, "\"");
		}
	}

	return r_strbuf_drain_nofree (&sb);
}