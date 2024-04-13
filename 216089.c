static size_t __str_ansi_length(char const *str) {
	size_t i = 1;
	if (str[0] == 0x1b) {
		if (str[1] == '[') {
			i++;
			while (str[i] && str[i] != 'J' && str[i] != 'm' && str[i] != 'H' && str[i] != 'K') {
				i++;
			}
		} else if (str[1] == '#') {
			while (str[i] && str[i] != 'q') {
				i++;
			}
		}
		if (str[i]) {
			i++;
		}
	}
	return i;
}