static int good_ipaddr(char *addr) {
	int dot_count;
	int digit_count;

	dot_count = 0;
	digit_count = 0;
	while(*addr != '\0' && *addr != ' ') {
		if (*addr == '.') {
			dot_count++;
			digit_count = 0;
		} else if (!isdigit(*addr)) {
			dot_count = 5;
		} else {
			digit_count++;
			if (digit_count > 3) {
				dot_count = 5;
			}
		}
		addr++;
	}
	if (dot_count != 3) {
		return -1;
	} else {
		return 0;
	}
}