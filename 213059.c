static const char *cfgget(RCore *core, const char *k) {
	return r_config_get (core->config, k);
}