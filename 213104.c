static void prompt_sec(RCore *r, char *s, size_t maxlen) {
	const RBinSection *sec = r_bin_get_section_at (r_bin_cur_object (r->bin), r->offset, true);
	if (!sec) {
		return;
	}
	r_str_ncpy (s, sec->name, maxlen - 2);
	strcat (s, ":");
}