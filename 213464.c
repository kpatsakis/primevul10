virtio_pci_legacy_write(struct vmctx *ctx, int vcpu, struct pci_vdev *dev,
			int baridx, uint64_t offset, int size, uint64_t value)
{
	struct virtio_base *base = dev->arg;
	struct virtio_vq_info *vq;
	struct virtio_ops *vops;
	const struct config_reg *cr;
	uint64_t virtio_config_size, max;
	const char *name;
	uint32_t newoff;
	int error = -1;


	if (base->mtx)
		pthread_mutex_lock(base->mtx);

	vops = base->vops;
	name = vops->name;

	if (size != 1 && size != 2 && size != 4)
		goto bad;

	if (pci_msix_enabled(dev))
		virtio_config_size = VIRTIO_PCI_CONFIG_OFF(1);
	else
		virtio_config_size = VIRTIO_PCI_CONFIG_OFF(0);

	if (offset >= virtio_config_size) {
		/*
		 * Subtract off the standard size (including MSI-X
		 * registers if enabled) and dispatch to underlying driver.
		 */
		newoff = offset - virtio_config_size;
		max = vops->cfgsize ? vops->cfgsize : 0x100000000;
		if (newoff + size > max)
			goto bad;
		if (vops->cfgwrite) {
			error = (*vops->cfgwrite)(DEV_STRUCT(base), newoff,
						  size, value);
		}
		if (!error)
			goto done;
	}

bad:
	cr = virtio_find_legacy_cr(offset);
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
		goto done;
	}

	switch (offset) {
	case VIRTIO_PCI_GUEST_FEATURES:
		base->negotiated_caps = value & base->device_caps;
		if (vops->apply_features)
			(*vops->apply_features)(DEV_STRUCT(base),
			    base->negotiated_caps);
		break;
	case VIRTIO_PCI_QUEUE_PFN:
		if (base->curq >= vops->nvq)
			goto bad_qindex;
		virtio_vq_init(base, value);
		break;
	case VIRTIO_PCI_QUEUE_SEL:
		/*
		 * Note that the guest is allowed to select an
		 * invalid queue; we just need to return a QNUM
		 * of 0 while the bad queue is selected.
		 */
		base->curq = value;
		break;
	case VIRTIO_PCI_QUEUE_NOTIFY:
		if (value >= vops->nvq) {
			pr_err("%s: queue %d notify out of range\r\n",
				name, (int)value);
			goto done;
		}
		vq = &base->queues[value];
		if (vq->notify)
			(*vq->notify)(DEV_STRUCT(base), vq);
		else if (vops->qnotify)
			(*vops->qnotify)(DEV_STRUCT(base), vq);
		else
			pr_err("%s: qnotify queue %d: missing vq/vops notify\r\n",
				name, (int)value);
		break;
	case VIRTIO_PCI_STATUS:
		base->status = value;
		if (vops->set_status)
			(*vops->set_status)(DEV_STRUCT(base), value);
		if ((value == 0) && (vops->reset))
			(*vops->reset)(DEV_STRUCT(base));
		if ((value & VIRTIO_CONFIG_S_DRIVER_OK) &&
		     base->backend_type == BACKEND_VBSU &&
		     virtio_poll_enabled) {
			base->polling_timer.clockid = CLOCK_MONOTONIC;
			acrn_timer_init(&base->polling_timer, virtio_poll_timer, base);
			/* wait 5s to start virtio poll mode
			 * skip vsbl and make sure device initialization completed
			 * FIXME: Need optimization in the future
			 */
			virtio_start_timer(&base->polling_timer, 5, 0);
		}
		break;
	case VIRTIO_MSI_CONFIG_VECTOR:
		base->msix_cfg_idx = value;
		break;
	case VIRTIO_MSI_QUEUE_VECTOR:
		if (base->curq >= vops->nvq)
			goto bad_qindex;
		vq = &base->queues[base->curq];
		vq->msix_idx = value;
		break;
	}
	goto done;

bad_qindex:
	pr_err("%s: write config reg %s: curq %d >= max %d\r\n",
	    name, cr->name, base->curq, vops->nvq);
done:
	if (base->mtx)
		pthread_mutex_unlock(base->mtx);
}