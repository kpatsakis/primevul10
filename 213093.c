static void r_core_debug_syscall_hit(RCore *core) {
	const char *cmdhit = r_config_get (core->config, "cmd.onsyscall");

	if (cmdhit && cmdhit[0] != 0) {
		r_core_cmd0 (core, cmdhit);
		r_cons_flush ();
	}
}