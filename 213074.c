R_API RCoreAutocomplete *r_core_autocomplete_find(RCoreAutocomplete *parent, const char* cmd, bool exact) {
	if (!parent || !cmd) {
		return false;
	}
	int len = strlen (cmd);
	int i;
	for (i = 0; i < parent->n_subcmds; i++) {
		if (exact && len == parent->subcmds[i]->length && !strncmp (cmd, parent->subcmds[i]->cmd, len)) {
			return parent->subcmds[i];
		} else if (!exact && !strncmp (cmd, parent->subcmds[i]->cmd, len)) {
			return parent->subcmds[i];
		}
	}
	return NULL;
}