ipv6_add_addr(struct inet6_dev *idev, const struct in6_addr *addr, int pfxlen,
	      int scope, u32 flags)
{
	struct inet6_ifaddr *ifa = NULL;
	struct rt6_info *rt;
	int hash;
	int err = 0;

	read_lock_bh(&addrconf_lock);
	if (idev->dead) {
		err = -ENODEV;			/*XXX*/
		goto out2;
	}

	write_lock(&addrconf_hash_lock);

	/* Ignore adding duplicate addresses on an interface */
	if (ipv6_chk_same_addr(addr, idev->dev)) {
		ADBG(("ipv6_add_addr: already assigned\n"));
		err = -EEXIST;
		goto out;
	}

	ifa = kmalloc(sizeof(struct inet6_ifaddr), GFP_ATOMIC);

	if (ifa == NULL) {
		ADBG(("ipv6_add_addr: malloc failed\n"));
		err = -ENOBUFS;
		goto out;
	}

	rt = addrconf_dst_alloc(idev, addr, 0);
	if (IS_ERR(rt)) {
		err = PTR_ERR(rt);
		goto out;
	}

	memset(ifa, 0, sizeof(struct inet6_ifaddr));
	ipv6_addr_copy(&ifa->addr, addr);

	spin_lock_init(&ifa->lock);
	init_timer(&ifa->timer);
	ifa->timer.data = (unsigned long) ifa;
	ifa->scope = scope;
	ifa->prefix_len = pfxlen;
	ifa->flags = flags | IFA_F_TENTATIVE;
	ifa->cstamp = ifa->tstamp = jiffies;

	ifa->idev = idev;
	in6_dev_hold(idev);
	/* For caller */
	in6_ifa_hold(ifa);

	/* Add to big hash table */
	hash = ipv6_addr_hash(addr);

	ifa->lst_next = inet6_addr_lst[hash];
	inet6_addr_lst[hash] = ifa;
	in6_ifa_hold(ifa);
	write_unlock(&addrconf_hash_lock);

	write_lock(&idev->lock);
	/* Add to inet6_dev unicast addr list. */
	ifa->if_next = idev->addr_list;
	idev->addr_list = ifa;

#ifdef CONFIG_IPV6_PRIVACY
	if (ifa->flags&IFA_F_TEMPORARY) {
		ifa->tmp_next = idev->tempaddr_list;
		idev->tempaddr_list = ifa;
		in6_ifa_hold(ifa);
	}
#endif

	ifa->rt = rt;

	in6_ifa_hold(ifa);
	write_unlock(&idev->lock);
out2:
	read_unlock_bh(&addrconf_lock);

	if (likely(err == 0))
		notifier_call_chain(&inet6addr_chain, NETDEV_UP, ifa);
	else {
		kfree(ifa);
		ifa = ERR_PTR(err);
	}

	return ifa;
out:
	write_unlock(&addrconf_hash_lock);
	goto out2;
}