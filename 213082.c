static bool r_core_anal_log(struct r_anal_t *anal, const char *msg) {
	RCore *core = anal->user;
	if (core->cfglog) {
		r_core_log_add (core, msg);
	}
	return true;
}