static bool find_e_opts(RCore *core, RLineCompletion *completion, RLineBuffer *buf) {
	const char *pattern = "e (.*)=";
	RRegex *rx = r_regex_new (pattern, "e");
	const size_t nmatch = 2;
	RRegexMatch pmatch[2];
	bool ret = false;

	// required to get the new list of items to autocomplete for cmd.pdc at least
	r_core_config_update (core);

	if (r_regex_exec (rx, buf->data, nmatch, pmatch, 1)) {
		goto out;
	}
	int i;
	char *str = NULL, *sp;
	for (i = pmatch[1].rm_so; i < pmatch[1].rm_eo; i++) {
		str = r_str_appendch (str, buf->data[i]);
	}
	if (!str) {
		goto out;
	}
	if ((sp = strchr (str, ' '))) {
		// if the name contains a space, just null
		*sp = 0;
	}
	RConfigNode *node = r_config_node_get (core->config, str);
	if (sp) {
		// if nulled, then restore.
		*sp = ' ';
	}
	if (!node) {
		return false;
	}
	RListIter *iter;
	char *option;
	char *p = (char *) strchr (buf->data, '=');
	p = r_str_ichr (p + 1, ' ');
	int n = strlen (p);
	r_list_foreach (node->options, iter, option) {
		if (!strncmp (option, p, n)) {
			r_line_completion_push (completion, option);
		}
	}
	completion->opt = true;
	ret = true;

 out:
	r_regex_free (rx);
	return ret;
}