static struct net_device *ipmr_reg_vif(void)
{
	struct net_device *dev;
	struct in_device *in_dev;

	dev = alloc_netdev(sizeof(struct net_device_stats), "pimreg",
			   reg_vif_setup);

	if (dev == NULL)
		return NULL;

	if (register_netdevice(dev)) {
		free_netdev(dev);
		return NULL;
	}
	dev->iflink = 0;

	if ((in_dev = inetdev_init(dev)) == NULL)
		goto failure;

	in_dev->cnf.rp_filter = 0;

	if (dev_open(dev))
		goto failure;

	return dev;

failure:
	/* allow the register to be completed before unregistering. */
	rtnl_unlock();
	rtnl_lock();

	unregister_netdevice(dev);
	return NULL;
}