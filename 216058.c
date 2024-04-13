R_API int r_str_casecmp(const char *s1, const char *s2) {
#ifdef _MSC_VER
	return stricmp (s1, s2);
#else
	return strcasecmp (s1, s2);
#endif
}