virtio_find_modern_cr(int offset) {
	return virtio_find_cr(modern_config_regs,
		sizeof(modern_config_regs) / sizeof(*modern_config_regs),
		offset);
}