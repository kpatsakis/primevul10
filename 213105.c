static void autocomplete_mount_point (RLineCompletion *completion, RCore *core, const char *path) {
	RFSRoot *r;
	RListIter *iter;
	r_list_foreach (core->fs->roots, iter, r) {
		char *base = strdup (r->path);
		char *ls = (char *) r_str_lchr (base, '/');
		if (ls) {
			ls++;
			*ls = 0;
		}
		if (!strcmp (path, base)) {
			r_line_completion_push (completion, r->path);
		}
		free (base);
	}
}