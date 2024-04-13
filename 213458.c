virtio_find_legacy_cr(int offset) {
	return virtio_find_cr(legacy_config_regs,
		sizeof(legacy_config_regs) / sizeof(*legacy_config_regs),
		offset);
}