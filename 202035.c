qdisc_create(struct net_device *dev, struct netdev_queue *dev_queue,
	     u32 parent, u32 handle, struct nlattr **tca, int *errp)
{
	int err;
	struct nlattr *kind = tca[TCA_KIND];
	struct Qdisc *sch;
	struct Qdisc_ops *ops;
	struct qdisc_size_table *stab;

	ops = qdisc_lookup_ops(kind);
#ifdef CONFIG_MODULES
	if (ops == NULL && kind != NULL) {
		char name[IFNAMSIZ];
		if (nla_strlcpy(name, kind, IFNAMSIZ) < IFNAMSIZ) {
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

	err = -ENOENT;
	if (ops == NULL)
		goto err_out;

	sch = qdisc_alloc(dev_queue, ops);
	if (IS_ERR(sch)) {
		err = PTR_ERR(sch);
		goto err_out2;
	}

	sch->parent = parent;

	if (handle == TC_H_INGRESS) {
		sch->flags |= TCQ_F_INGRESS;
		handle = TC_H_MAKE(TC_H_INGRESS, 0);
		lockdep_set_class(qdisc_lock(sch), &qdisc_rx_lock);
	} else {
		if (handle == 0) {
			handle = qdisc_alloc_handle(dev);
			err = -ENOMEM;
			if (handle == 0)
				goto err_out3;
		}
		lockdep_set_class(qdisc_lock(sch), &qdisc_tx_lock);
	}

	sch->handle = handle;

	if (!ops->init || (err = ops->init(sch, tca[TCA_OPTIONS])) == 0) {
		if (tca[TCA_STAB]) {
			stab = qdisc_get_stab(tca[TCA_STAB]);
			if (IS_ERR(stab)) {
				err = PTR_ERR(stab);
				goto err_out3;
			}
			sch->stab = stab;
		}
		if (tca[TCA_RATE]) {
			spinlock_t *root_lock;

			if ((sch->parent != TC_H_ROOT) &&
			    !(sch->flags & TCQ_F_INGRESS))
				root_lock = qdisc_root_sleeping_lock(sch);
			else
				root_lock = qdisc_lock(sch);

			err = gen_new_estimator(&sch->bstats, &sch->rate_est,
						root_lock, tca[TCA_RATE]);
			if (err) {
				/*
				 * Any broken qdiscs that would require
				 * a ops->reset() here? The qdisc was never
				 * in action so it shouldn't be necessary.
				 */
				if (ops->destroy)
					ops->destroy(sch);
				goto err_out3;
			}
		}

		qdisc_list_add(sch);

		return sch;
	}
err_out3:
	qdisc_put_stab(sch->stab);
	dev_put(dev);
	kfree((char *) sch - sch->padded);
err_out2:
	module_put(ops->owner);
err_out:
	*errp = err;
	return NULL;
}