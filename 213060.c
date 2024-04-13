static void autocomplete_macro(RCore *core, RLineCompletion *completion, const char *str) {
	r_return_if_fail (str);
	RCmdMacroItem *item;
	RListIter *iter;
	char buf[1024];
	int n = strlen(str);
	r_list_foreach (core->rcmd->macro.macros, iter, item) {
		char *p = item->name;
		if (!*str || !strncmp (str, p, n)) {
			snprintf (buf, sizeof (buf), "%s%s)", str, p);
			r_line_completion_push (completion, buf);
		}
	}
}