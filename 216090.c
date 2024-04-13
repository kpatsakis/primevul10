static int checkcmd(const char *c) {
	char oc = 0;
	for (;*c;c++) {
		if (oc == '%') {
			if (*c != 'd' && *c != '%') {
				return 0;
			}
		}
		oc = *c;
	}
	return 1;
}