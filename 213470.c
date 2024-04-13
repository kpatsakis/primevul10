virtio_linkup(struct virtio_base *base, struct virtio_ops *vops,
	      void *pci_virtio_dev, struct pci_vdev *dev,
	      struct virtio_vq_info *queues,
	      int backend_type)
{
	int i;

	/* base and pci_virtio_dev addresses must match */
	if ((void *)base != pci_virtio_dev) {
		pr_err("virtio_base and pci_virtio_dev addresses don't match!\n");
		return;
	}
	base->vops = vops;
	base->dev = dev;
	dev->arg = base;
	base->backend_type = backend_type;

	base->queues = queues;
	for (i = 0; i < vops->nvq; i++) {
		queues[i].base = base;
		queues[i].num = i;
	}
}