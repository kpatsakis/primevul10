static void addrconf_forward_change(void)
{
	struct net_device *dev;
	struct inet6_dev *idev;

	read_lock(&dev_base_lock);
	for (dev=dev_base; dev; dev=dev->next) {
		read_lock(&addrconf_lock);
		idev = __in6_dev_get(dev);
		if (idev) {
			int changed = (!idev->cnf.forwarding) ^ (!ipv6_devconf.forwarding);
			idev->cnf.forwarding = ipv6_devconf.forwarding;
			if (changed)
				dev_forward_change(idev);
		}
		read_unlock(&addrconf_lock);
	}
	read_unlock(&dev_base_lock);
}