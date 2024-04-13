static void dev_forward_change(struct inet6_dev *idev)
{
	struct net_device *dev;
	struct inet6_ifaddr *ifa;
	struct in6_addr addr;

	if (!idev)
		return;
	dev = idev->dev;
	if (dev && (dev->flags & IFF_MULTICAST)) {
		ipv6_addr_all_routers(&addr);
	
		if (idev->cnf.forwarding)
			ipv6_dev_mc_inc(dev, &addr);
		else
			ipv6_dev_mc_dec(dev, &addr);
	}
	for (ifa=idev->addr_list; ifa; ifa=ifa->if_next) {
		if (idev->cnf.forwarding)
			addrconf_join_anycast(ifa);
		else
			addrconf_leave_anycast(ifa);
	}
}