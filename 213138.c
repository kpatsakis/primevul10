R_API void r_core_free(RCore *c) {
	if (c) {
		r_core_fini (c);
		free (c);
	}
}