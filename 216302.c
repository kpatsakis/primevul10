R_API size_t r_str_nlen_w(const char *str, int n) {
	size_t len = 0;
	if (str) {
		while (*str && n > 0) {
			len++;
			str++;
			if (!*str) {
				//handle wide strings
			 	//xx00yy00bb00
				if (n - 2 > 0) {
					if (str[2]) {
						break;
					}
				}
				str++;
			}
			n--;
		}
	}
	return len;
}