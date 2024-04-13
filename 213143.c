static void autocomplete_file(RLineCompletion *completion, const char *str) {
	r_return_if_fail (str);
	char *pipe = strchr (str, '>');

	if (pipe) {
		str = r_str_trim_head_ro (pipe + 1);
	}
	if (str && !*str) {
		autocomplete_process_path (completion, str, "./");
	} else {
		autocomplete_process_path (completion, str, str);
	}

}