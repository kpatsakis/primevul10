R_API bool r_core_autocomplete_remove(RCoreAutocomplete *parent, const char* cmd) {
	if (!parent || !cmd) {
		return false;
	}
	int i, j;
	for (i = 0; i < parent->n_subcmds; i++) {
		RCoreAutocomplete *ac = parent->subcmds[i];
		if (ac->locked) {
			continue;
		}
		// if (!strncmp (parent->subcmds[i]->cmd, cmd, parent->subcmds[i]->length)) {
		if (r_str_glob (ac->cmd, cmd)) {
			for (j = i + 1; j < parent->n_subcmds; j++) {
				parent->subcmds[j - 1] = parent->subcmds[j];
				parent->subcmds[j] = NULL;
			}
			r_core_autocomplete_free (ac);
			RCoreAutocomplete **updated = realloc (parent->subcmds, (parent->n_subcmds - 1) * sizeof (RCoreAutocomplete*));
			if (!updated && (parent->n_subcmds - 1) > 0) {
				eprintf ("Something really bad has happen.. this should never ever happen..\n");
				return false;
			}
			parent->subcmds = updated;
			parent->n_subcmds--;
			i--;
		}
	}
	return false;
}