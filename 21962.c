static void stack_map_free(struct bpf_map *map)
{
	struct bpf_stack_map *smap = container_of(map, struct bpf_stack_map, map);

	bpf_map_area_free(smap->elems);
	pcpu_freelist_destroy(&smap->freelist);
	bpf_map_area_free(smap);
	put_callchain_buffers();
}