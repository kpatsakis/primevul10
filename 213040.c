static const char *colorfor_cb(void *user, ut64 addr, bool verbose) {
	return r_core_anal_optype_colorfor ((RCore *)user, addr, verbose);
}