static void autocomplete_ms_file(RCore* core, RLineCompletion *completion, const char *str) {
	r_return_if_fail (str);
	char *pipe = strchr (str, '>');
	char *path = (core->rfs && *(core->rfs->cwd)) ? *(core->rfs->cwd): "/";
	if (pipe) {
		str = r_str_trim_head_ro (pipe + 1);
	}
	if (str && !*str) {
		autocomplete_ms_path (completion, core, str, path);
	} else {
		autocomplete_ms_path (completion, core, str, str);
	}
}