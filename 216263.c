R_API ut32 r_str_hash(const char *s) {
	return (ut32) r_str_hash64 (s);
}