R_API bool r_str_is_true(const char *s) {
	return !r_str_casecmp ("yes", s)
		|| !r_str_casecmp ("on", s)
		|| !r_str_casecmp ("true", s)
		|| !r_str_casecmp ("1", s);
}