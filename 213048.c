static void setab(RCore *core, const char *arch, int bits) {
	if (arch) {
		r_config_set (core->config, "asm.arch", arch);
	}
	if (bits > 0) {
		r_config_set_i (core->config, "asm.bits", bits);
	}
}