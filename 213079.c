static char *hasrefs_cb(void *user, ut64 addr, bool verbose) {
	return r_core_anal_hasrefs ((RCore *)user, addr, verbose);
}