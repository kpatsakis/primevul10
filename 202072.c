static int ipv6_create_tempaddr(struct inet6_ifaddr *ifp, struct inet6_ifaddr *ift)
{
	struct inet6_dev *idev = ifp->idev;
	struct in6_addr addr, *tmpaddr;
	unsigned long tmp_prefered_lft, tmp_valid_lft, tmp_cstamp, tmp_tstamp;
	int tmp_plen;
	int ret = 0;
	int max_addresses;

	write_lock(&idev->lock);
	if (ift) {
		spin_lock_bh(&ift->lock);
		memcpy(&addr.s6_addr[8], &ift->addr.s6_addr[8], 8);
		spin_unlock_bh(&ift->lock);
		tmpaddr = &addr;
	} else {
		tmpaddr = NULL;
	}
retry:
	in6_dev_hold(idev);
	if (idev->cnf.use_tempaddr <= 0) {
		write_unlock(&idev->lock);
		printk(KERN_INFO
			"ipv6_create_tempaddr(): use_tempaddr is disabled.\n");
		in6_dev_put(idev);
		ret = -1;
		goto out;
	}
	spin_lock_bh(&ifp->lock);
	if (ifp->regen_count++ >= idev->cnf.regen_max_retry) {
		idev->cnf.use_tempaddr = -1;	/*XXX*/
		spin_unlock_bh(&ifp->lock);
		write_unlock(&idev->lock);
		printk(KERN_WARNING
			"ipv6_create_tempaddr(): regeneration time exceeded. disabled temporary address support.\n");
		in6_dev_put(idev);
		ret = -1;
		goto out;
	}
	in6_ifa_hold(ifp);
	memcpy(addr.s6_addr, ifp->addr.s6_addr, 8);
	if (__ipv6_try_regen_rndid(idev, tmpaddr) < 0) {
		spin_unlock_bh(&ifp->lock);
		write_unlock(&idev->lock);
		printk(KERN_WARNING
			"ipv6_create_tempaddr(): regeneration of randomized interface id failed.\n");
		in6_ifa_put(ifp);
		in6_dev_put(idev);
		ret = -1;
		goto out;
	}
	memcpy(&addr.s6_addr[8], idev->rndid, 8);
	tmp_valid_lft = min_t(__u32,
			      ifp->valid_lft,
			      idev->cnf.temp_valid_lft);
	tmp_prefered_lft = min_t(__u32, 
				 ifp->prefered_lft, 
				 idev->cnf.temp_prefered_lft - desync_factor / HZ);
	tmp_plen = ifp->prefix_len;
	max_addresses = idev->cnf.max_addresses;
	tmp_cstamp = ifp->cstamp;
	tmp_tstamp = ifp->tstamp;
	spin_unlock_bh(&ifp->lock);

	write_unlock(&idev->lock);
	ift = !max_addresses ||
	      ipv6_count_addresses(idev) < max_addresses ? 
		ipv6_add_addr(idev, &addr, tmp_plen,
			      ipv6_addr_type(&addr)&IPV6_ADDR_SCOPE_MASK, IFA_F_TEMPORARY) : NULL;
	if (!ift || IS_ERR(ift)) {
		in6_ifa_put(ifp);
		in6_dev_put(idev);
		printk(KERN_INFO
			"ipv6_create_tempaddr(): retry temporary address regeneration.\n");
		tmpaddr = &addr;
		write_lock(&idev->lock);
		goto retry;
	}

	spin_lock_bh(&ift->lock);
	ift->ifpub = ifp;
	ift->valid_lft = tmp_valid_lft;
	ift->prefered_lft = tmp_prefered_lft;
	ift->cstamp = tmp_cstamp;
	ift->tstamp = tmp_tstamp;
	spin_unlock_bh(&ift->lock);

	addrconf_dad_start(ift, 0);
	in6_ifa_put(ift);
	in6_dev_put(idev);
out:
	return ret;
}