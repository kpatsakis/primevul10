static void autocomplete_flagspaces(RCore *core, RLineCompletion *completion, const char* msg) {
	r_return_if_fail (msg);
	int length = strlen (msg);
	RFlag *flag = core->flags;
	RSpaceIter it;
	RSpace *s;
	r_flag_space_foreach (flag, it, s) {
		if (!strncmp (msg, s->name, length)) {
			r_line_completion_push (completion, s->name);
		}
	}

	if (strlen (msg) == 0) {
		r_line_completion_push (completion, "*");
	}
}