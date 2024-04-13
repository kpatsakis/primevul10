R_API int r_str_delta(char *p, char a, char b) {
	char *_a = strchr (p, a);
	char *_b = strchr (p, b);
	return (!_a || !_b)? 0 : (_a - _b);
}