R_API bool r_str_startswith(const char *str, const char *needle) {
	r_return_val_if_fail (str && needle, false);
	if (str == needle) {
		return true;
	}
	return !strncmp (str, needle, strlen (needle));
}