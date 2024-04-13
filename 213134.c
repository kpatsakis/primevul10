static const char *getName(RCore *core, ut64 addr) {
	RFlagItem *item = r_flag_get_i (core->flags, addr);
	if (item) {
		if (core->flags->realnames) {
			return item->realname
				? item->realname: item->name;
		}
		return item->name;
	}
	return NULL;
}