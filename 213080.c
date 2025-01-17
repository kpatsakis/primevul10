static void autocomplete_sdb (RCore *core, RLineCompletion *completion, const char *str) {
	r_return_if_fail (core && completion && str);
	char *pipe = strchr (str, '>');
	Sdb *sdb = core->sdb;
	char *lpath = NULL, *p1 = NULL, *out = NULL, *p2 = NULL;
	char *cur_pos = NULL, *cur_cmd = NULL, *next_cmd = NULL;
	char *temp_cmd = NULL, *temp_pos = NULL, *key = NULL;
	if (pipe) {
		str = r_str_trim_head_ro (pipe + 1);
	}
	lpath = r_str_new (str);
	p1 = strchr (lpath, '/');
	if (p1) {
		*p1 = 0;
		char *ns = p1 + 1;
		p2 = strchr (ns, '/');
		if (!p2) { // anal/m
			char *tmp = p1 + 1;
			int n = strlen (tmp);
			out = sdb_querys (sdb, NULL, 0, "anal/**");
			if (!out) {
				return;
			}
			while (*out) {
				cur_pos = strchr (out, '\n');
				if (!cur_pos) {
					break;
				}
				cur_cmd = r_str_ndup (out, cur_pos - out);
				if (!strncmp (tmp, cur_cmd, n)) {
					char *cmplt = r_str_newf ("anal/%s/", cur_cmd);
					r_line_completion_push (completion, cmplt);
					free (cmplt);
				}
				out += cur_pos - out + 1;
			}

		} else { // anal/meta/*
			char *tmp = p2 + 1;
			int n = strlen (tmp);
			char *spltr = strchr (ns, '/');
			*spltr = 0;
			next_cmd = r_str_newf ("anal/%s/*", ns);
			out = sdb_querys (sdb, NULL, 0, next_cmd);
			if (!out) {
				free (lpath);
				return;
			}
			while (*out) {
				temp_pos = strchr (out, '\n');
				if (!temp_pos) {
					break;
				}
				temp_cmd = r_str_ndup (out, temp_pos - out); // contains the key=value pair
				key = strchr (temp_cmd, '=');
				*key = 0;
				if (!strncmp (tmp, temp_cmd, n)) {
					char *cmplt = r_str_newf ("anal/%s/%s", ns, temp_cmd);
					r_line_completion_push (completion, cmplt);
					free (cmplt);
				}
				out += temp_pos - out + 1;
			}
		}
	} else {
		int n = strlen (lpath);
		if (!strncmp (lpath, "anal", n)) {
			r_line_completion_push (completion, "anal/");
		}
	}
}