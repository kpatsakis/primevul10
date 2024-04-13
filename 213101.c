static void autocompleteFilename(RLineCompletion *completion, RLineBuffer *buf, char **extra_paths, int narg) {
	char *args = NULL, *input = NULL;
	int n = 0, i = 0;
	char *pipe = strchr (buf->data, '>');
	if (pipe) {
		args = r_str_new (pipe + 1);
	} else {
		args = r_str_new (buf->data);
	}
	if (!args) {
		goto out;
	}

	n = r_str_word_set0 (args);
	if (n < narg) {
		goto out;
	}

	input = r_str_new (r_str_word_get0 (args, narg));
	if (!input) {
		goto out;
	}
	const char *tinput = r_str_trim_head_ro (input);

	autocomplete_process_path (completion, buf->data, tinput);

	if (input[0] == '/' || input[0] == '.' || !extra_paths) {
		goto out;
	}

	for (i = 0; extra_paths[i]; i ++) {
		char *s = r_str_newf ("%s%s%s", extra_paths[i], R_SYS_DIR, tinput);
		if (!s) {
			break;
		}
		autocomplete_process_path (completion, buf->data, s);
		free (s);
	}
out:
	free (args);
	free (input);
}