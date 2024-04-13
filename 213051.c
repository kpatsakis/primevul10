static void autocomplete_theme(RCore *core, RLineCompletion *completion, const char *str) {
	r_return_if_fail (str);
	int len = strlen (str);
	char *theme;
	RListIter *iter;
	RList *themes = r_core_list_themes (core);
	r_list_foreach (themes, iter, theme) {
		if (!len || !strncmp (str, theme, len)) {
			r_line_completion_push (completion, theme);
		}
	}
	r_list_free (themes);
}