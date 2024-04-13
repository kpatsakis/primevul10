static int addrconf_notify(struct notifier_block *this, unsigned long event, 
			   void * data)
{
	struct net_device *dev = (struct net_device *) data;
	struct inet6_dev *idev = __in6_dev_get(dev);

	switch(event) {
	case NETDEV_UP:
		switch(dev->type) {
		case ARPHRD_SIT:
			addrconf_sit_config(dev);
			break;
		case ARPHRD_TUNNEL6:
			addrconf_ip6_tnl_config(dev);
			break;
		case ARPHRD_LOOPBACK:
			init_loopback(dev);
			break;

		default:
			addrconf_dev_config(dev);
			break;
		};
		if (idev) {
			/* If the MTU changed during the interface down, when the
			   interface up, the changed MTU must be reflected in the
			   idev as well as routers.
			 */
			if (idev->cnf.mtu6 != dev->mtu && dev->mtu >= IPV6_MIN_MTU) {
				rt6_mtu_change(dev, dev->mtu);
				idev->cnf.mtu6 = dev->mtu;
			}
			idev->tstamp = jiffies;
			inet6_ifinfo_notify(RTM_NEWLINK, idev);
			/* If the changed mtu during down is lower than IPV6_MIN_MTU
			   stop IPv6 on this interface.
			 */
			if (dev->mtu < IPV6_MIN_MTU)
				addrconf_ifdown(dev, event != NETDEV_DOWN);
		}
		break;

	case NETDEV_CHANGEMTU:
		if ( idev && dev->mtu >= IPV6_MIN_MTU) {
			rt6_mtu_change(dev, dev->mtu);
			idev->cnf.mtu6 = dev->mtu;
			break;
		}

		/* MTU falled under IPV6_MIN_MTU. Stop IPv6 on this interface. */

	case NETDEV_DOWN:
	case NETDEV_UNREGISTER:
		/*
		 *	Remove all addresses from this interface.
		 */
		addrconf_ifdown(dev, event != NETDEV_DOWN);
		break;
	case NETDEV_CHANGE:
		break;
	case NETDEV_CHANGENAME:
#ifdef CONFIG_SYSCTL
		if (idev) {
			addrconf_sysctl_unregister(&idev->cnf);
			neigh_sysctl_unregister(idev->nd_parms);
			neigh_sysctl_register(dev, idev->nd_parms,
					      NET_IPV6, NET_IPV6_NEIGH, "ipv6",
					      &ndisc_ifinfo_sysctl_change,
					      NULL);
			addrconf_sysctl_register(idev, &idev->cnf);
		}
#endif
		break;
	};

	return NOTIFY_OK;
}