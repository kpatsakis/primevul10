static int bpf_map_direct_read(struct bpf_map *map, int off, int size, u64 *val)
{
	void *ptr;
	u64 addr;
	int err;

	err = map->ops->map_direct_value_addr(map, &addr, off);
	if (err)
		return err;
	ptr = (void *)(long)addr + off;

	switch (size) {
	case sizeof(u8):
		*val = (u64)*(u8 *)ptr;
		break;
	case sizeof(u16):
		*val = (u64)*(u16 *)ptr;
		break;
	case sizeof(u32):
		*val = (u64)*(u32 *)ptr;
		break;
	case sizeof(u64):
		*val = *(u64 *)ptr;
		break;
	default:
		return -EINVAL;
	}
	return 0;
}