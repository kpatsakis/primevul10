static const char *get_section_name(void *user, ut64 addr) {
	return r_core_get_section_name ((RCore *)user, addr);
}