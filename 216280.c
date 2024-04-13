R_API int r_str_do_until_token(str_operation op, char *str, const char tok) {
	int ret;
	if (!str) {
		return -1;
	}
	if (!op) {
		for (ret = 0; (str[ret] != tok) && str[ret]; ret++) {
			//empty body
		}
	} else {
		for (ret = 0; (str[ret] != tok) && str[ret]; ret++) {
			op (str + ret);
		}
	}
	return ret;
}