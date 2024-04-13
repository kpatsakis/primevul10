virtio_common_cfg_write(struct pci_vdev *dev, uint64_t offset, int size,
			uint64_t value)
{
	struct virtio_base *base = dev->arg;
	struct virtio_vq_info *vq;
	struct virtio_ops *vops;
	const struct config_reg *cr;
	const char *name;

	vops = base->vops;
	name = vops->name;

	cr = virtio_find_modern_cr(offset);
	if (cr == NULL || cr->size != size || cr->ro) {
		if (cr != NULL) {
			/* offset must be OK, wrong size and/or reg is R/O */
			if (cr->size != size)
				pr_err("%s: write to %s: bad size %d\r\n",
					name, cr->name, size);
			if (cr->ro)
				pr_err("%s: write to read-only reg %s\r\n",
					name, cr->name);
		} else {
			pr_err("%s: write to bad offset/size %jd/%d\r\n",
				name, (uintmax_t)offset, size);
		}

		return;
	}

	switch (offset) {
	case VIRTIO_PCI_COMMON_DFSELECT:
		base->device_feature_select = value;
		break;
	case VIRTIO_PCI_COMMON_GFSELECT:
		base->driver_feature_select = value;
		break;
	case VIRTIO_PCI_COMMON_GF:
		if (base->status & VIRTIO_CONFIG_S_DRIVER_OK)
			break;
		if (base->driver_feature_select < 2) {
			value &= 0xffffffff;
			base->negotiated_caps =
				(value << (base->driver_feature_select * 32))
				& base->device_caps;
			if (vops->apply_features)
				(*vops->apply_features)(DEV_STRUCT(base),
					base->negotiated_caps);
		}
		break;
	case VIRTIO_PCI_COMMON_MSIX:
		base->msix_cfg_idx = value;
		break;
	case VIRTIO_PCI_COMMON_STATUS:
		base->status = value & 0xff;
		if (vops->set_status)
			(*vops->set_status)(DEV_STRUCT(base), value);
		if ((base->status == 0) && (vops->reset))
			(*vops->reset)(DEV_STRUCT(base));
		/* TODO: virtio poll mode for modern devices */
		break;
	case VIRTIO_PCI_COMMON_Q_SELECT:
		/*
		 * Note that the guest is allowed to select an
		 * invalid queue; we just need to return a QNUM
		 * of 0 while the bad queue is selected.
		 */
		base->curq = value;
		break;
	case VIRTIO_PCI_COMMON_Q_SIZE:
		if (base->curq >= vops->nvq)
			goto bad_qindex;
		vq = &base->queues[base->curq];
		vq->qsize = value;
		break;
	case VIRTIO_PCI_COMMON_Q_MSIX:
		if (base->curq >= vops->nvq)
			goto bad_qindex;
		vq = &base->queues[base->curq];
		vq->msix_idx = value;
		break;
	case VIRTIO_PCI_COMMON_Q_ENABLE:
		if (base->curq >= vops->nvq)
			goto bad_qindex;
		virtio_vq_enable(base);
		break;
	case VIRTIO_PCI_COMMON_Q_DESCLO:
		if (base->curq >= vops->nvq)
			goto bad_qindex;
		vq = &base->queues[base->curq];
		vq->gpa_desc[0] = value;
		break;
	case VIRTIO_PCI_COMMON_Q_DESCHI:
		if (base->curq >= vops->nvq)
			goto bad_qindex;
		vq = &base->queues[base->curq];
		vq->gpa_desc[1] = value;
		break;
	case VIRTIO_PCI_COMMON_Q_AVAILLO:
		if (base->curq >= vops->nvq)
			goto bad_qindex;
		vq = &base->queues[base->curq];
		vq->gpa_avail[0] = value;
		break;
	case VIRTIO_PCI_COMMON_Q_AVAILHI:
		if (base->curq >= vops->nvq)
			goto bad_qindex;
		vq = &base->queues[base->curq];
		vq->gpa_avail[1] = value;
		break;
	case VIRTIO_PCI_COMMON_Q_USEDLO:
		if (base->curq >= vops->nvq)
			goto bad_qindex;
		vq = &base->queues[base->curq];
		vq->gpa_used[0] = value;
		break;
	case VIRTIO_PCI_COMMON_Q_USEDHI:
		if (base->curq >= vops->nvq)
			goto bad_qindex;
		vq = &base->queues[base->curq];
		vq->gpa_used[1] = value;
		break;
	}

	return;

bad_qindex:
	pr_err("%s: write config reg %s: curq %d >= max %d\r\n",
		name, cr->name, base->curq, vops->nvq);
}