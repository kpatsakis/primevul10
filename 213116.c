static void autocomplete_project(RCore *core, RLineCompletion *completion, const char* str) {
	r_return_if_fail (str);
	char *foo, *projects_path = r_file_abspath (r_config_get (core->config, "dir.projects"));
	RList *list = r_sys_dir (projects_path);
	RListIter *iter;
	int n = strlen (str);
	if (projects_path) {
		r_list_foreach (list, iter, foo) {
			if (r_core_is_project (core, foo)) {
				if (!strncmp (foo, str, n)) {
					r_line_completion_push (completion, foo);
				}
			}
		}
		free (projects_path);
		r_list_free (list);
	}
}