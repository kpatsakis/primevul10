R_API int r_str_ncasecmp(const char *s1, const char *s2, size_t n) {
#ifdef _MSC_VER
	return _strnicmp (s1, s2, n);
#else
	return strncasecmp (s1, s2, n);
#endif
}