R_API char* r_core_add_asmqjmp(RCore *core, ut64 addr) {
	bool found = false;
	if (!core->asmqjmps) {
		return NULL;
	}
	if (core->is_asmqjmps_letter) {
		if (core->asmqjmps_count >= R_CORE_ASMQJMPS_MAX_LETTERS) {
			return NULL;
		}
		if (core->asmqjmps_count >= core->asmqjmps_size - 2) {
			core->asmqjmps = realloc (core->asmqjmps, core->asmqjmps_size * 2 * sizeof (ut64));
			if (!core->asmqjmps) {
				return NULL;
			}
			core->asmqjmps_size *= 2;
		}
	}
	if (core->asmqjmps_count < core->asmqjmps_size - 1) {
		int i = 0;
		char t[R_CORE_ASMQJMPS_LEN_LETTERS + 1] = {0};
		for (i = 0; i < core->asmqjmps_count + 1; i++) {
			if (core->asmqjmps[i] == addr) {
				found = true;
				break;
			}
		}
		if (!found) {
			i = ++core->asmqjmps_count;
			core->asmqjmps[i] = addr;
		}
		// This check makes pos never be <1, thefor not fill 't' with trash
		if (i < 1) {
			return NULL;
		}
		r_core_set_asmqjmps (core, t, sizeof (t), i);
		return strdup (t);
	}
	return NULL;
}