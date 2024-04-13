R_API bool r_str_endswith(const char *str, const char *needle) {
	r_return_val_if_fail (str && needle, false);
	if (!*needle) {
		return true;
	}
	int slen = strlen (str);
	int nlen = strlen (needle);
	if (!slen || !nlen || slen < nlen) {
		return false;
	}
	return !strcmp (str + (slen - nlen), needle);
}