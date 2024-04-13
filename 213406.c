static bool is_preallocated_map(struct bpf_map *map)
{
	if (!check_map_prealloc(map))
		return false;
	if (map->inner_map_meta && !check_map_prealloc(map->inner_map_meta))
		return false;
	return true;
}