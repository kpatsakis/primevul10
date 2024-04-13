R_API RCore *r_core_new() {
	RCore *c = R_NEW0 (RCore);
	if (c) {
		r_core_init (c);
	}
	return c;
}