static int inet6_addr_add(int ifindex, struct in6_addr *pfx, int plen)
{
	struct inet6_ifaddr *ifp;
	struct inet6_dev *idev;
	struct net_device *dev;
	int scope;

	ASSERT_RTNL();
	
	if ((dev = __dev_get_by_index(ifindex)) == NULL)
		return -ENODEV;
	
	if (!(dev->flags&IFF_UP))
		return -ENETDOWN;

	if ((idev = addrconf_add_dev(dev)) == NULL)
		return -ENOBUFS;

	scope = ipv6_addr_scope(pfx);

	ifp = ipv6_add_addr(idev, pfx, plen, scope, IFA_F_PERMANENT);
	if (!IS_ERR(ifp)) {
		addrconf_dad_start(ifp, 0);
		in6_ifa_put(ifp);
		return 0;
	}

	return PTR_ERR(ifp);
}