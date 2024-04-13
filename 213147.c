static int autocomplete_pfele (RCore *core, RLineCompletion *completion, char *key, char *pfx, int idx, char *ptr) {
	int i, ret = 0;
	int len = strlen (ptr);
	char* fmt = sdb_get (core->print->formats, key, NULL);
	if (fmt) {
		int nargs = r_str_word_set0_stack (fmt);
		if (nargs > 1) {
			for (i = 1; i < nargs; i++) {
				const char *arg = r_str_word_get0 (fmt, i);
				char *p = strchr (arg, '(');
				char *p2 = strchr (arg, ')');
				// remove '(' and ')' from fmt
				if (p && p2) {
					arg = p + 1;
					*p2 = '\0';
				}
				if (!len || !strncmp (ptr, arg, len)) {
					char *s = r_str_newf ("pf%s.%s.%s", pfx, key, arg);
					r_line_completion_push (completion, s);
					free (s);
				}
			}
		}
	}
	free (fmt);
	return ret;
}