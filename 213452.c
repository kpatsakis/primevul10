virtio_device_cfg_read(struct pci_vdev *dev, uint64_t offset, int size)
{
	struct virtio_base *base = dev->arg;
	struct virtio_ops *vops;
	const char *name;
	uint32_t value;
	uint64_t max;
	int error = -1;

	vops = base->vops;
	name = vops->name;
	value = size == 1 ? 0xff : size == 2 ? 0xffff : 0xffffffff;
	max = vops->cfgsize ? vops->cfgsize : 0x100000000;

	if (offset + size > max) {
		pr_err("%s: reading from 0x%lx size %d exceeds limit\r\n",
			name, offset, size);
		return value;
	}

	if (vops->cfgread) {
		error = (*vops->cfgread)(DEV_STRUCT(base), offset, size, &value);
	}
	if (error) {
		pr_err("%s: reading from 0x%lx size %d failed %d\r\n",
			name, offset, size, error);
		value = size == 1 ? 0xff : size == 2 ? 0xffff : 0xffffffff;
	}

	return value;
}