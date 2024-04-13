void addrconf_prefix_rcv(struct net_device *dev, u8 *opt, int len)
{
	struct prefix_info *pinfo;
	__u32 valid_lft;
	__u32 prefered_lft;
	int addr_type;
	unsigned long rt_expires;
	struct inet6_dev *in6_dev;

	pinfo = (struct prefix_info *) opt;
	
	if (len < sizeof(struct prefix_info)) {
		ADBG(("addrconf: prefix option too short\n"));
		return;
	}
	
	/*
	 *	Validation checks ([ADDRCONF], page 19)
	 */

	addr_type = ipv6_addr_type(&pinfo->prefix);

	if (addr_type & (IPV6_ADDR_MULTICAST|IPV6_ADDR_LINKLOCAL))
		return;

	valid_lft = ntohl(pinfo->valid);
	prefered_lft = ntohl(pinfo->prefered);

	if (prefered_lft > valid_lft) {
		if (net_ratelimit())
			printk(KERN_WARNING "addrconf: prefix option has invalid lifetime\n");
		return;
	}

	in6_dev = in6_dev_get(dev);

	if (in6_dev == NULL) {
		if (net_ratelimit())
			printk(KERN_DEBUG "addrconf: device %s not configured\n", dev->name);
		return;
	}

	/*
	 *	Two things going on here:
	 *	1) Add routes for on-link prefixes
	 *	2) Configure prefixes with the auto flag set
	 */

	/* Avoid arithmetic overflow. Really, we could
	   save rt_expires in seconds, likely valid_lft,
	   but it would require division in fib gc, that it
	   not good.
	 */
	if (valid_lft >= 0x7FFFFFFF/HZ)
		rt_expires = 0;
	else
		rt_expires = jiffies + valid_lft * HZ;

	if (pinfo->onlink) {
		struct rt6_info *rt;
		rt = rt6_lookup(&pinfo->prefix, NULL, dev->ifindex, 1);

		if (rt && ((rt->rt6i_flags & (RTF_GATEWAY | RTF_DEFAULT)) == 0)) {
			if (rt->rt6i_flags&RTF_EXPIRES) {
				if (valid_lft == 0) {
					ip6_del_rt(rt, NULL, NULL, NULL);
					rt = NULL;
				} else {
					rt->rt6i_expires = rt_expires;
				}
			}
		} else if (valid_lft) {
			addrconf_prefix_route(&pinfo->prefix, pinfo->prefix_len,
					      dev, rt_expires, RTF_ADDRCONF|RTF_EXPIRES|RTF_PREFIX_RT);
		}
		if (rt)
			dst_release(&rt->u.dst);
	}

	/* Try to figure out our local address for this prefix */

	if (pinfo->autoconf && in6_dev->cnf.autoconf) {
		struct inet6_ifaddr * ifp;
		struct in6_addr addr;
		int create = 0, update_lft = 0;

		if (pinfo->prefix_len == 64) {
			memcpy(&addr, &pinfo->prefix, 8);
			if (ipv6_generate_eui64(addr.s6_addr + 8, dev) &&
			    ipv6_inherit_eui64(addr.s6_addr + 8, in6_dev)) {
				in6_dev_put(in6_dev);
				return;
			}
			goto ok;
		}
		if (net_ratelimit())
			printk(KERN_DEBUG "IPv6 addrconf: prefix with wrong length %d\n",
			       pinfo->prefix_len);
		in6_dev_put(in6_dev);
		return;

ok:

		ifp = ipv6_get_ifaddr(&addr, dev, 1);

		if (ifp == NULL && valid_lft) {
			int max_addresses = in6_dev->cnf.max_addresses;

			/* Do not allow to create too much of autoconfigured
			 * addresses; this would be too easy way to crash kernel.
			 */
			if (!max_addresses ||
			    ipv6_count_addresses(in6_dev) < max_addresses)
				ifp = ipv6_add_addr(in6_dev, &addr, pinfo->prefix_len,
						    addr_type&IPV6_ADDR_SCOPE_MASK, 0);

			if (!ifp || IS_ERR(ifp)) {
				in6_dev_put(in6_dev);
				return;
			}

			update_lft = create = 1;
			ifp->cstamp = jiffies;
			addrconf_dad_start(ifp, RTF_ADDRCONF|RTF_PREFIX_RT);
		}

		if (ifp) {
			int flags;
			unsigned long now;
#ifdef CONFIG_IPV6_PRIVACY
			struct inet6_ifaddr *ift;
#endif
			u32 stored_lft;

			/* update lifetime (RFC2462 5.5.3 e) */
			spin_lock(&ifp->lock);
			now = jiffies;
			if (ifp->valid_lft > (now - ifp->tstamp) / HZ)
				stored_lft = ifp->valid_lft - (now - ifp->tstamp) / HZ;
			else
				stored_lft = 0;
			if (!update_lft && stored_lft) {
				if (valid_lft > MIN_VALID_LIFETIME ||
				    valid_lft > stored_lft)
					update_lft = 1;
				else if (stored_lft <= MIN_VALID_LIFETIME) {
					/* valid_lft <= stored_lft is always true */
					/* XXX: IPsec */
					update_lft = 0;
				} else {
					valid_lft = MIN_VALID_LIFETIME;
					if (valid_lft < prefered_lft)
						prefered_lft = valid_lft;
					update_lft = 1;
				}
			}

			if (update_lft) {
				ifp->valid_lft = valid_lft;
				ifp->prefered_lft = prefered_lft;
				ifp->tstamp = now;
				flags = ifp->flags;
				ifp->flags &= ~IFA_F_DEPRECATED;
				spin_unlock(&ifp->lock);

				if (!(flags&IFA_F_TENTATIVE))
					ipv6_ifa_notify(0, ifp);
			} else
				spin_unlock(&ifp->lock);

#ifdef CONFIG_IPV6_PRIVACY
			read_lock_bh(&in6_dev->lock);
			/* update all temporary addresses in the list */
			for (ift=in6_dev->tempaddr_list; ift; ift=ift->tmp_next) {
				/*
				 * When adjusting the lifetimes of an existing
				 * temporary address, only lower the lifetimes.
				 * Implementations must not increase the
				 * lifetimes of an existing temporary address
				 * when processing a Prefix Information Option.
				 */
				spin_lock(&ift->lock);
				flags = ift->flags;
				if (ift->valid_lft > valid_lft &&
				    ift->valid_lft - valid_lft > (jiffies - ift->tstamp) / HZ)
					ift->valid_lft = valid_lft + (jiffies - ift->tstamp) / HZ;
				if (ift->prefered_lft > prefered_lft &&
				    ift->prefered_lft - prefered_lft > (jiffies - ift->tstamp) / HZ)
					ift->prefered_lft = prefered_lft + (jiffies - ift->tstamp) / HZ;
				spin_unlock(&ift->lock);
				if (!(flags&IFA_F_TENTATIVE))
					ipv6_ifa_notify(0, ift);
			}

			if (create && in6_dev->cnf.use_tempaddr > 0) {
				/*
				 * When a new public address is created as described in [ADDRCONF],
				 * also create a new temporary address.
				 */
				read_unlock_bh(&in6_dev->lock); 
				ipv6_create_tempaddr(ifp, NULL);
			} else {
				read_unlock_bh(&in6_dev->lock);
			}
#endif
			in6_ifa_put(ifp);
			addrconf_verify(0);
		}
	}
	inet6_prefix_notify(RTM_NEWPREFIX, in6_dev, pinfo);
	in6_dev_put(in6_dev);
}