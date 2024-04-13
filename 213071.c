static char *get_comments_cb(void *user, ut64 addr) {
	return r_core_anal_get_comments ((RCore *)user, addr);
}