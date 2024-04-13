R_API void r_core_autocomplete_reload (RCore *core) {
	r_return_if_fail (core);
	r_core_autocomplete_free (core->autocomplete);
	__init_autocomplete (core);
}