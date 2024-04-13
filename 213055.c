static void autocomplete_evals(RCore *core, RLineCompletion *completion, const char *str) {
	r_return_if_fail (str);
	RConfigNode *bt;
	RListIter *iter;
	char *tmp = strrchr (str, ' ');
	if (tmp) {
		str = tmp + 1;
	}
	int n = strlen (str);
	if (n < 1) {
		return;
	}
	r_list_foreach (core->config->nodes, iter, bt) {
		if (!strncmp (bt->name, str, n)) {
			r_line_completion_push (completion, bt->name);
		}
	}
}