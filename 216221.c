R_API bool r_str_isnumber(const char *str) {
	if (!str || !*str) {
		return false;
	}
	bool isnum = IS_DIGIT (*str) || *str == '-';
	while (isnum && *++str) {
		if (!IS_DIGIT (*str)) {
			isnum = false;
		}
	}
	return isnum;
}