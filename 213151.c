static bool __syncDebugMaps(RCore *core) {
	if (r_config_get_i (core->config, "cfg.debug")) {
		return r_debug_map_sync (core->dbg);
	}
	return false;
}