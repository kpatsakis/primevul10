static void __ipv6_ifa_notify(int event, struct inet6_ifaddr *ifp)
{
	inet6_ifa_notify(event ? : RTM_NEWADDR, ifp);

	switch (event) {
	case RTM_NEWADDR:
		dst_hold(&ifp->rt->u.dst);
		if (ip6_ins_rt(ifp->rt, NULL, NULL, NULL))
			dst_release(&ifp->rt->u.dst);
		if (ifp->idev->cnf.forwarding)
			addrconf_join_anycast(ifp);
		break;
	case RTM_DELADDR:
		if (ifp->idev->cnf.forwarding)
			addrconf_leave_anycast(ifp);
		addrconf_leave_solict(ifp->idev, &ifp->addr);
		dst_hold(&ifp->rt->u.dst);
		if (ip6_del_rt(ifp->rt, NULL, NULL, NULL))
			dst_free(&ifp->rt->u.dst);
		else
			dst_release(&ifp->rt->u.dst);
		break;
	}
}