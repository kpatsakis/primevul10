qdisc_create(struct net_device *dev, u32 handle, struct rtattr **tca, int *errp)
{
	int err;
	struct rtattr *kind = tca[TCA_KIND-1];
	void *p = NULL;
	struct Qdisc *sch;
	struct Qdisc_ops *ops;
	int size;

	ops = qdisc_lookup_ops(kind);
#ifdef CONFIG_KMOD
	if (ops == NULL && kind != NULL) {
		char name[IFNAMSIZ];
		if (rtattr_strlcpy(name, kind, IFNAMSIZ) < IFNAMSIZ) {
			/* We dropped the RTNL semaphore in order to
			 * perform the module load.  So, even if we
			 * succeeded in loading the module we have to
			 * tell the caller to replay the request.  We
			 * indicate this using -EAGAIN.
			 * We replay the request because the device may
			 * go away in the mean time.
			 */
			rtnl_unlock();
			request_module("sch_%s", name);
			rtnl_lock();
			ops = qdisc_lookup_ops(kind);
			if (ops != NULL) {
				/* We will try again qdisc_lookup_ops,
				 * so don't keep a reference.
				 */
				module_put(ops->owner);
				err = -EAGAIN;
				goto err_out;
			}
		}
	}
#endif

	err = -EINVAL;
	if (ops == NULL)
		goto err_out;

	/* ensure that the Qdisc and the private data are 32-byte aligned */
	size = ((sizeof(*sch) + QDISC_ALIGN_CONST) & ~QDISC_ALIGN_CONST);
	size += ops->priv_size + QDISC_ALIGN_CONST;

	p = kmalloc(size, GFP_KERNEL);
	err = -ENOBUFS;
	if (!p)
		goto err_out2;
	memset(p, 0, size);
	sch = (struct Qdisc *)(((unsigned long)p + QDISC_ALIGN_CONST)
	                       & ~QDISC_ALIGN_CONST);
	sch->padded = (char *)sch - (char *)p;

	INIT_LIST_HEAD(&sch->list);
	skb_queue_head_init(&sch->q);

	if (handle == TC_H_INGRESS)
		sch->flags |= TCQ_F_INGRESS;

	sch->ops = ops;
	sch->enqueue = ops->enqueue;
	sch->dequeue = ops->dequeue;
	sch->dev = dev;
	dev_hold(dev);
	atomic_set(&sch->refcnt, 1);
	sch->stats_lock = &dev->queue_lock;
	if (handle == 0) {
		handle = qdisc_alloc_handle(dev);
		err = -ENOMEM;
		if (handle == 0)
			goto err_out3;
	}

	if (handle == TC_H_INGRESS)
                sch->handle =TC_H_MAKE(TC_H_INGRESS, 0);
        else
                sch->handle = handle;

	if (!ops->init || (err = ops->init(sch, tca[TCA_OPTIONS-1])) == 0) {
		qdisc_lock_tree(dev);
		list_add_tail(&sch->list, &dev->qdisc_list);
		qdisc_unlock_tree(dev);

#ifdef CONFIG_NET_ESTIMATOR
		if (tca[TCA_RATE-1])
			gen_new_estimator(&sch->bstats, &sch->rate_est,
				sch->stats_lock, tca[TCA_RATE-1]);
#endif
		return sch;
	}
err_out3:
	dev_put(dev);
err_out2:
	module_put(ops->owner);
err_out:
	*errp = err;
	if (p)
		kfree(p);
	return NULL;
}