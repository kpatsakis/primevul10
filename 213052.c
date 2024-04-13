R_API char *r_core_anal_hasrefs(RCore *core, ut64 value, bool verbose) {
	if (verbose) {
		const int hex_depth = r_config_get_i (core->config, "hex.depth");
		return r_core_anal_hasrefs_to_depth (core, value, hex_depth);
	}
	RFlagItem *fi = r_flag_get_i (core->flags, value);
	return fi? strdup (fi->name): NULL;
}