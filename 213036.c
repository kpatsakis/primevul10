R_API ut64 r_core_get_asmqjmps(RCore *core, const char *str) {
	if (!core->asmqjmps) {
		return UT64_MAX;
	}
	if (core->is_asmqjmps_letter) {
		int i, pos = 0;
		int len = strlen (str);
		for (i = 0; i < len - 1; i++) {
			if (!isupper ((ut8)str[i])) {
				return UT64_MAX;
			}
			pos *= R_CORE_ASMQJMPS_LETTERS;
			pos += str[i] - 'A' + 1;
		}
		if (!islower ((ut8)str[i])) {
			return UT64_MAX;
		}
		pos *= R_CORE_ASMQJMPS_LETTERS;
		pos += str[i] - 'a';
		if (pos < core->asmqjmps_count) {
			return core->asmqjmps[pos + 1];
		}
	} else if (str[0] > '0' && str[1] <= '9') {
		int pos = str[0] - '0';
		if (pos <= core->asmqjmps_count) {
			return core->asmqjmps[pos];
		}
	}
	return UT64_MAX;
}