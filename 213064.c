R_API void r_core_autocomplete_free(RCoreAutocomplete *obj) {
	if (!obj) {
		return;
	}
	int i;
	for (i = 0; i < obj->n_subcmds; i++) {
		r_core_autocomplete_free (obj->subcmds[i]);
		obj->subcmds[i] = NULL;
	}
	free (obj->subcmds);
	free ((char*) obj->cmd);
	free (obj);
}