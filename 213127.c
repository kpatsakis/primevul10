static void autocomplete_default(R_NULLABLE RCore *core, RLineCompletion *completion, RLineBuffer *buf) {
	RCoreAutocomplete *a = core ? core->autocomplete : NULL;
	int i;
	if (a) {
		for (i = 0; i < a->n_subcmds; i++) {
			if (buf->data[0] == 0 || !strncmp (a->subcmds[i]->cmd, buf->data, a->subcmds[i]->length)) {
				r_line_completion_push (completion, a->subcmds[i]->cmd);
			}
		}
	} else {
		for (i = 0; i < radare_argc && radare_argv[i]; i++) {
			int length = strlen (radare_argv[i]);
			if (!strncmp (radare_argv[i], buf->data, length)) {
				r_line_completion_push (completion, radare_argv[i]);
			}
		}
	}
}