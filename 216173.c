R_API int r_str_arg_unescape(char *arg) {
	int dest_i = 0, src_i = 0;
	if (!arg) {
		return 0;
	}
	for (src_i = 0; arg[src_i] != '\0'; src_i++) {
		char c = arg[src_i];
		if (c == '\\') {
			if (arg[++src_i] == '\0') {
				break;
			}
			arg[dest_i++] = arg[src_i];
		} else {
			arg[dest_i++] = c;
		}
	}
	arg[dest_i] = '\0';
	return dest_i;
}