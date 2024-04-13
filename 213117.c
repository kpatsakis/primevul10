static void r_core_debug_breakpoint_hit(RCore *core, RBreakpointItem *bpi) {
	const char *cmdbp = r_config_get (core->config, "cmd.bp");
	const bool cmdbp_exists = (cmdbp && *cmdbp);
	const bool bpcmd_exists = (bpi->data && bpi->data[0]);
	const bool may_output = (cmdbp_exists || bpcmd_exists);
	if (may_output) {
		r_cons_push ();
	}
	if (cmdbp_exists) {
		r_core_cmd0 (core, cmdbp);
	}
	if (bpcmd_exists) {
		r_core_cmd0 (core, bpi->data);
	}
	if (may_output) {
		r_cons_flush ();
		r_cons_pop ();
	}
}