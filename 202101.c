int ipv6_dev_get_saddr(struct net_device *dev,
		       struct in6_addr *daddr, struct in6_addr *saddr)
{
	struct inet6_ifaddr *ifp = NULL;
	struct inet6_ifaddr *match = NULL;
	struct inet6_dev *idev;
	int scope;
	int err;
	int hiscore = -1, score;

	scope = ipv6_addr_scope(daddr);

	/*
	 *	known dev
	 *	search dev and walk through dev addresses
	 */

	if (dev) {
		if (dev->flags & IFF_LOOPBACK)
			scope = IFA_HOST;

		read_lock(&addrconf_lock);
		idev = __in6_dev_get(dev);
		if (idev) {
			read_lock_bh(&idev->lock);
			for (ifp=idev->addr_list; ifp; ifp=ifp->if_next) {
				if (ifp->scope == scope) {
					if (ifp->flags&IFA_F_TENTATIVE)
						continue;
#ifdef CONFIG_IPV6_PRIVACY
					score = ipv6_saddr_pref(ifp, idev->cnf.use_tempaddr > 1 ? IFA_F_TEMPORARY : 0);
#else
					score = ipv6_saddr_pref(ifp, 0);
#endif
					if (score <= hiscore)
						continue;

					if (match)
						in6_ifa_put(match);
					match = ifp;
					hiscore = score;
					in6_ifa_hold(ifp);

					if (IPV6_GET_SADDR_MAXSCORE(score)) {
						read_unlock_bh(&idev->lock);
						read_unlock(&addrconf_lock);
						goto out;
					}
				}
			}
			read_unlock_bh(&idev->lock);
		}
		read_unlock(&addrconf_lock);
	}

	if (scope == IFA_LINK)
		goto out;

	/*
	 *	dev == NULL or search failed for specified dev
	 */

	read_lock(&dev_base_lock);
	read_lock(&addrconf_lock);
	for (dev = dev_base; dev; dev=dev->next) {
		idev = __in6_dev_get(dev);
		if (idev) {
			read_lock_bh(&idev->lock);
			for (ifp=idev->addr_list; ifp; ifp=ifp->if_next) {
				if (ifp->scope == scope) {
					if (ifp->flags&IFA_F_TENTATIVE)
						continue;
#ifdef CONFIG_IPV6_PRIVACY
					score = ipv6_saddr_pref(ifp, idev->cnf.use_tempaddr > 1 ? IFA_F_TEMPORARY : 0);
#else
					score = ipv6_saddr_pref(ifp, 0);
#endif
					if (score <= hiscore)
						continue;

					if (match)
						in6_ifa_put(match);
					match = ifp;
					hiscore = score;
					in6_ifa_hold(ifp);

					if (IPV6_GET_SADDR_MAXSCORE(score)) {
						read_unlock_bh(&idev->lock);
						goto out_unlock_base;
					}
				}
			}
			read_unlock_bh(&idev->lock);
		}
	}

out_unlock_base:
	read_unlock(&addrconf_lock);
	read_unlock(&dev_base_lock);

out:
	err = -EADDRNOTAVAIL;
	if (match) {
		ipv6_addr_copy(saddr, &match->addr);
		err = 0;
		in6_ifa_put(match);
	}

	return err;
}