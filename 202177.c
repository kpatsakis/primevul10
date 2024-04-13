static struct inet6_dev * ipv6_add_dev(struct net_device *dev)
{
	struct inet6_dev *ndev;

	ASSERT_RTNL();

	if (dev->mtu < IPV6_MIN_MTU)
		return NULL;

	ndev = kmalloc(sizeof(struct inet6_dev), GFP_KERNEL);

	if (ndev) {
		memset(ndev, 0, sizeof(struct inet6_dev));

		rwlock_init(&ndev->lock);
		ndev->dev = dev;
		memcpy(&ndev->cnf, &ipv6_devconf_dflt, sizeof(ndev->cnf));
		ndev->cnf.mtu6 = dev->mtu;
		ndev->cnf.sysctl = NULL;
		ndev->nd_parms = neigh_parms_alloc(dev, &nd_tbl);
		if (ndev->nd_parms == NULL) {
			kfree(ndev);
			return NULL;
		}
		/* We refer to the device */
		dev_hold(dev);

		if (snmp6_alloc_dev(ndev) < 0) {
			ADBG((KERN_WARNING
				"%s(): cannot allocate memory for statistics; dev=%s.\n",
				__FUNCTION__, dev->name));
			neigh_parms_release(&nd_tbl, ndev->nd_parms);
			ndev->dead = 1;
			in6_dev_finish_destroy(ndev);
			return NULL;
		}

		if (snmp6_register_dev(ndev) < 0) {
			ADBG((KERN_WARNING
				"%s(): cannot create /proc/net/dev_snmp6/%s\n",
				__FUNCTION__, dev->name));
			neigh_parms_release(&nd_tbl, ndev->nd_parms);
			ndev->dead = 1;
			in6_dev_finish_destroy(ndev);
			return NULL;
		}

		/* One reference from device.  We must do this before
		 * we invoke __ipv6_regen_rndid().
		 */
		in6_dev_hold(ndev);

#ifdef CONFIG_IPV6_PRIVACY
		get_random_bytes(ndev->rndid, sizeof(ndev->rndid));
		get_random_bytes(ndev->entropy, sizeof(ndev->entropy));
		init_timer(&ndev->regen_timer);
		ndev->regen_timer.function = ipv6_regen_rndid;
		ndev->regen_timer.data = (unsigned long) ndev;
		if ((dev->flags&IFF_LOOPBACK) ||
		    dev->type == ARPHRD_TUNNEL ||
		    dev->type == ARPHRD_NONE ||
		    dev->type == ARPHRD_SIT) {
			printk(KERN_INFO
				"Disabled Privacy Extensions on device %p(%s)\n",
				dev, dev->name);
			ndev->cnf.use_tempaddr = -1;
		} else {
			in6_dev_hold(ndev);
			ipv6_regen_rndid((unsigned long) ndev);
		}
#endif

		write_lock_bh(&addrconf_lock);
		dev->ip6_ptr = ndev;
		write_unlock_bh(&addrconf_lock);

		ipv6_mc_init_dev(ndev);
		ndev->tstamp = jiffies;
#ifdef CONFIG_SYSCTL
		neigh_sysctl_register(dev, ndev->nd_parms, NET_IPV6, 
				      NET_IPV6_NEIGH, "ipv6",
				      &ndisc_ifinfo_sysctl_change,
				      NULL);
		addrconf_sysctl_register(ndev, &ndev->cnf);
#endif
	}
	return ndev;
}