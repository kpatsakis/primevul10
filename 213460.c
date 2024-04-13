virtio_device_cfg_write(struct pci_vdev *dev, uint64_t offset, int size,
			uint64_t value)
{
	struct virtio_base *base = dev->arg;
	struct virtio_ops *vops;
	const char *name;
	uint64_t max;
	int error = -1;

	vops = base->vops;
	name = vops->name;
	max = vops->cfgsize ? vops->cfgsize : 0x100000000;

	if (offset + size > max) {
		pr_err("%s: writing to 0x%lx size %d exceeds limit\r\n",
			name, offset, size);
		return;
	}

	if (vops->cfgwrite) {
		error = (*vops->cfgwrite)(DEV_STRUCT(base), offset, size, value);
	}
	if (error)
		pr_err("%s: writing ot 0x%lx size %d failed %d\r\n",
			name, offset, size, error);
}