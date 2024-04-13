R_API bool r_str_cmp_list(const char *list, const char *item, char sep) {
	if (!list || !item) {
		return false;
	}
	int i = 0, j = 0;
	for (; list[i] && list[i] != sep; i++, j++) {
		if (item[j] != list[i]) {
			while (list[i] && list[i] != sep) {
				i++;
			}
			if (!list[i]) {
				return false;
			}
			j = -1;
			continue;
		}
	}
	return true;
}