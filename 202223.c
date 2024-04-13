int neigh_sysctl_register(struct net_device *dev, struct neigh_parms *p,
			  int p_id, int pdev_id, char *p_name, 
			  proc_handler *handler, ctl_handler *strategy)
{
	struct neigh_sysctl_table *t = kmalloc(sizeof(*t), GFP_KERNEL);
	const char *dev_name_source = NULL;
	char *dev_name = NULL;
	int err = 0;

	if (!t)
		return -ENOBUFS;
	memcpy(t, &neigh_sysctl_template, sizeof(*t));
	t->neigh_vars[0].data  = &p->mcast_probes;
	t->neigh_vars[1].data  = &p->ucast_probes;
	t->neigh_vars[2].data  = &p->app_probes;
	t->neigh_vars[3].data  = &p->retrans_time;
	t->neigh_vars[4].data  = &p->base_reachable_time;
	t->neigh_vars[5].data  = &p->delay_probe_time;
	t->neigh_vars[6].data  = &p->gc_staletime;
	t->neigh_vars[7].data  = &p->queue_len;
	t->neigh_vars[8].data  = &p->proxy_qlen;
	t->neigh_vars[9].data  = &p->anycast_delay;
	t->neigh_vars[10].data = &p->proxy_delay;
	t->neigh_vars[11].data = &p->locktime;

	if (dev) {
		dev_name_source = dev->name;
		t->neigh_dev[0].ctl_name = dev->ifindex;
		t->neigh_vars[12].procname = NULL;
		t->neigh_vars[13].procname = NULL;
		t->neigh_vars[14].procname = NULL;
		t->neigh_vars[15].procname = NULL;
	} else {
 		dev_name_source = t->neigh_dev[0].procname;
		t->neigh_vars[12].data = (int *)(p + 1);
		t->neigh_vars[13].data = (int *)(p + 1) + 1;
		t->neigh_vars[14].data = (int *)(p + 1) + 2;
		t->neigh_vars[15].data = (int *)(p + 1) + 3;
	}

	t->neigh_vars[16].data  = &p->retrans_time;
	t->neigh_vars[17].data  = &p->base_reachable_time;

	if (handler || strategy) {
		/* RetransTime */
		t->neigh_vars[3].proc_handler = handler;
		t->neigh_vars[3].strategy = strategy;
		t->neigh_vars[3].extra1 = dev;
		/* ReachableTime */
		t->neigh_vars[4].proc_handler = handler;
		t->neigh_vars[4].strategy = strategy;
		t->neigh_vars[4].extra1 = dev;
		/* RetransTime (in milliseconds)*/
		t->neigh_vars[16].proc_handler = handler;
		t->neigh_vars[16].strategy = strategy;
		t->neigh_vars[16].extra1 = dev;
		/* ReachableTime (in milliseconds) */
		t->neigh_vars[17].proc_handler = handler;
		t->neigh_vars[17].strategy = strategy;
		t->neigh_vars[17].extra1 = dev;
	}

	dev_name = kstrdup(dev_name_source, GFP_KERNEL);
	if (!dev_name) {
		err = -ENOBUFS;
		goto free;
	}

 	t->neigh_dev[0].procname = dev_name;

	t->neigh_neigh_dir[0].ctl_name = pdev_id;

	t->neigh_proto_dir[0].procname = p_name;
	t->neigh_proto_dir[0].ctl_name = p_id;

	t->neigh_dev[0].child	       = t->neigh_vars;
	t->neigh_neigh_dir[0].child    = t->neigh_dev;
	t->neigh_proto_dir[0].child    = t->neigh_neigh_dir;
	t->neigh_root_dir[0].child     = t->neigh_proto_dir;

	t->sysctl_header = register_sysctl_table(t->neigh_root_dir, 0);
	if (!t->sysctl_header) {
		err = -ENOBUFS;
		goto free_procname;
	}
	p->sysctl_table = t;
	return 0;

	/* error path */
 free_procname:
	kfree(dev_name);
 free:
	kfree(t);

	return err;
}