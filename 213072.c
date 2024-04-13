static void autocomplete_minus(RCore *core, RLineCompletion *completion, const char *str) {
	r_return_if_fail (str);
	int count;
	int length = strlen (str);
	char **keys = r_cmd_alias_keys(core->rcmd, &count);
	if (!keys) {
		return;
	}
	int i;
	for (i = 0; i < count; i++) {
		if (!strncmp (keys[i], str, length)) {
			r_line_completion_push (completion, keys[i]);
		}
	}
}