static void autocomplete_zignatures(RCore *core, RLineCompletion *completion, const char* msg) {
	r_return_if_fail (msg);
	int length = strlen (msg);
	RSpaces *zs = &core->anal->zign_spaces;
	RSpace *s;
	RSpaceIter it;

	r_spaces_foreach (zs, it, s) {
		if (!strncmp (msg, s->name, length)) {
			r_line_completion_push (completion, s->name);
		}
	}

	if (strlen (msg) == 0) {
		r_line_completion_push (completion, "*");
	}
}