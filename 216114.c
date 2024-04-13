R_API bool r_str_is_false(const char *s) {
	return !r_str_casecmp ("no", s)
		|| !r_str_casecmp ("off", s)
		|| !r_str_casecmp ("false", s)
		|| !r_str_casecmp ("0", s)
		|| !*s;
}