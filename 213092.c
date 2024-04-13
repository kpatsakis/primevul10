static bool __isMapped(RCore *core, ut64 addr, int perm) {
	if (r_config_get_i (core->config, "cfg.debug")) {
		// RList *maps = core->dbg->maps;
		RDebugMap *map = NULL;
		RListIter *iter = NULL;

		r_list_foreach (core->dbg->maps, iter, map) {
			if (addr >= map->addr && addr < map->addr_end) {
				if (perm > 0) {
					if (map->perm & perm) {
						return true;
					}
				} else {
					return true;
				}
			}
		}

		return false;
	}

	return r_io_map_is_mapped (core->io, addr);
}