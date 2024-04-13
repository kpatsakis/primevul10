static void ipv6_ifa_notify(int event, struct inet6_ifaddr *ifp)
{
	read_lock_bh(&addrconf_lock);
	if (likely(ifp->idev->dead == 0))
		__ipv6_ifa_notify(event, ifp);
	read_unlock_bh(&addrconf_lock);
}