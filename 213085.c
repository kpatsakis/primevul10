static int cfggeti(RCore *core, const char *k) {
	return r_config_get_i (core->config, k);
}