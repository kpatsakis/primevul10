static void __init_autocomplete (RCore* core) {
	int i;
	core->autocomplete = R_NEW0 (RCoreAutocomplete);
	if (core->autocomplete_type == AUTOCOMPLETE_DEFAULT) {
		__init_autocomplete_default (core);
	} else if (core->autocomplete_type == AUTOCOMPLETE_MS) {
		r_core_autocomplete_add (core->autocomplete, "ls", R_CORE_AUTOCMPLT_MS, true);
		r_core_autocomplete_add (core->autocomplete, "cd", R_CORE_AUTOCMPLT_MS, true);
		r_core_autocomplete_add (core->autocomplete, "cat", R_CORE_AUTOCMPLT_MS, true);
		r_core_autocomplete_add (core->autocomplete, "get", R_CORE_AUTOCMPLT_MS, true);
		r_core_autocomplete_add (core->autocomplete, "mount", R_CORE_AUTOCMPLT_MS, true);
		for (i = 0; i < ms_argc && ms_argv[i]; i++) {
			if (!r_core_autocomplete_find (core->autocomplete, ms_argv[i], true)) {
				r_core_autocomplete_add (core->autocomplete, ms_argv[i], R_CORE_AUTOCMPLT_MS, true);
			}
		}
	}
}