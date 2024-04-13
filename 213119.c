R_API RCoreAutocomplete *r_core_autocomplete_add(RCoreAutocomplete *parent, const char* cmd, int type, bool lock) {
	if (!parent || !cmd || type < 0 || type >= R_CORE_AUTOCMPLT_END) {
		return NULL;
	}
	RCoreAutocomplete *autocmpl = R_NEW0 (RCoreAutocomplete);
	if (!autocmpl) {
		return NULL;
	}
	RCoreAutocomplete **updated = realloc (parent->subcmds, (parent->n_subcmds + 1) * sizeof (RCoreAutocomplete*));
	if (!updated) {
		free (autocmpl);
		return NULL;
	}
	parent->subcmds = updated;
	parent->subcmds[parent->n_subcmds] = autocmpl;
	parent->n_subcmds++;
	autocmpl->cmd = strdup (cmd);
	autocmpl->locked = lock;
	autocmpl->type = type;
	autocmpl->length = strlen (cmd);
	return autocmpl;
}