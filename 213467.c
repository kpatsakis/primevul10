virtio_common_cfg_read(struct pci_vdev *dev, uint64_t offset, int size)
{
	struct virtio_base *base = dev->arg;
	struct virtio_ops *vops;
	const struct config_reg *cr;
	const char *name;
	uint32_t value;

	vops = base->vops;
	name = vops->name;
	value = size == 1 ? 0xff : size == 2 ? 0xffff : 0xffffffff;

	cr = virtio_find_modern_cr(offset);
	if (cr == NULL || cr->size != size) {
		if (cr != NULL) {
			/* offset must be OK, so size must be bad */
			pr_err("%s: read from %s: bad size %d\r\n",
				name, cr->name, size);
		} else {
			pr_err("%s: read from bad offset/size %jd/%d\r\n",
				name, (uintmax_t)offset, size);
		}

		return value;
	}

	switch (offset) {
	case VIRTIO_PCI_COMMON_DFSELECT:
		value = base->device_feature_select;
		break;
	case VIRTIO_PCI_COMMON_DF:
		if (base->device_feature_select == 0)
			value = base->device_caps & 0xffffffff;
		else if (base->device_feature_select == 1)
			value = (base->device_caps >> 32) & 0xffffffff;
		else /* present 0, see 4.1.4.3.1 */
			value = 0;
		break;
	case VIRTIO_PCI_COMMON_GFSELECT:
		value = base->driver_feature_select;
		break;
	case VIRTIO_PCI_COMMON_GF:
		/* see 4.1.4.3.1. Present any valid feature bits the driver
		 * has written in driver_feature. Valid feature bits are those
		 * which are subset of the corresponding device_feature bits
		 */
		if (base->driver_feature_select == 0)
			value = base->negotiated_caps & 0xffffffff;
		else if (base->driver_feature_select == 1)
			value = (base->negotiated_caps >> 32) & 0xffffffff;
		else
			value = 0;
		break;
	case VIRTIO_PCI_COMMON_MSIX:
		value = base->msix_cfg_idx;
		break;
	case VIRTIO_PCI_COMMON_NUMQ:
		value = vops->nvq;
		break;
	case VIRTIO_PCI_COMMON_STATUS:
		value = base->status;
		break;
	case VIRTIO_PCI_COMMON_CFGGENERATION:
		value = base->config_generation;
		break;
	case VIRTIO_PCI_COMMON_Q_SELECT:
		value = base->curq;
		break;
	case VIRTIO_PCI_COMMON_Q_SIZE:
		value = base->curq < vops->nvq ?
			base->queues[base->curq].qsize : 0;
		break;
	case VIRTIO_PCI_COMMON_Q_MSIX:
		value = base->curq < vops->nvq ?
			base->queues[base->curq].msix_idx :
			VIRTIO_MSI_NO_VECTOR;
		break;
	case VIRTIO_PCI_COMMON_Q_ENABLE:
		value = base->curq < vops->nvq ?
			base->queues[base->curq].enabled : 0;
		break;
	case VIRTIO_PCI_COMMON_Q_NOFF:
		value = base->curq;
		break;
	case VIRTIO_PCI_COMMON_Q_DESCLO:
		value = base->curq < vops->nvq ?
			base->queues[base->curq].gpa_desc[0] : 0;
		break;
	case VIRTIO_PCI_COMMON_Q_DESCHI:
		value = base->curq < vops->nvq ?
			base->queues[base->curq].gpa_desc[1] : 0;
		break;
	case VIRTIO_PCI_COMMON_Q_AVAILLO:
		value = base->curq < vops->nvq ?
			base->queues[base->curq].gpa_avail[0] : 0;
		break;
	case VIRTIO_PCI_COMMON_Q_AVAILHI:
		value = base->curq < vops->nvq ?
			base->queues[base->curq].gpa_avail[1] : 0;
		break;
	case VIRTIO_PCI_COMMON_Q_USEDLO:
		value = base->curq < vops->nvq ?
			base->queues[base->curq].gpa_used[0] : 0;
		break;
	case VIRTIO_PCI_COMMON_Q_USEDHI:
		value = base->curq < vops->nvq ?
			base->queues[base->curq].gpa_used[1] : 0;
		break;
	}

	return value;
}