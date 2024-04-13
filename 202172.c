void addrconf_join_anycast(struct inet6_ifaddr *ifp)
{
	struct in6_addr addr;
	ipv6_addr_prefix(&addr, &ifp->addr, ifp->prefix_len);
	if (ipv6_addr_any(&addr))
		return;
	ipv6_dev_ac_inc(ifp->idev->dev, &addr);
}