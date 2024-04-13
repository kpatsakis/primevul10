R_API bool r_str_is_bool(const char *val) {
	return r_str_is_true (val) || r_str_is_false (val);
}