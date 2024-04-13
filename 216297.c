R_API void r_str_case(char *str, bool up) {
	if (up) {
		char oc = 0;
		for (; *str; oc = *str++) {
			*str = (*str=='x' && oc=='0') ? 'x': toupper ((int)(ut8)*str);
		}
	} else {
		for (; *str; str++) {
			*str = tolower ((int)(ut8)*str);
		}
	}
}