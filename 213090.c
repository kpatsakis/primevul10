static char *core_cmdstr_callback (void *user, const char *cmd) {
	RCore *core = (RCore *)user;
	return r_core_cmd_str (core, cmd);
}