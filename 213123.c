static void __foreach(RCore *core, const char **cmds, int type) {
	int i;
	for (i = 0; cmds[i]; i++) {
		r_core_autocomplete_add (core->autocomplete, cmds[i], type, true);
	}
}