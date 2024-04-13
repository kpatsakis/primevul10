static char *getNameDelta(RCore *core, ut64 addr) {
	RFlagItem *item = r_flag_get_at (core->flags, addr, true);
	if (item) {
		if (item->offset != addr) {
			return r_str_newf ("%s + %d", item->name, (int)(addr - item->offset));
		}
		return strdup (item->name);
	}
	return NULL;
}