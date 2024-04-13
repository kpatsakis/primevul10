static int inet6_fill_ifaddr(struct sk_buff *skb, struct inet6_ifaddr *ifa,
			     u32 pid, u32 seq, int event, unsigned int flags)
{
	struct ifaddrmsg *ifm;
	struct nlmsghdr  *nlh;
	struct ifa_cacheinfo ci;
	unsigned char	 *b = skb->tail;

	nlh = NLMSG_NEW(skb, pid, seq, event, sizeof(*ifm), flags);
	ifm = NLMSG_DATA(nlh);
	ifm->ifa_family = AF_INET6;
	ifm->ifa_prefixlen = ifa->prefix_len;
	ifm->ifa_flags = ifa->flags;
	ifm->ifa_scope = RT_SCOPE_UNIVERSE;
	if (ifa->scope&IFA_HOST)
		ifm->ifa_scope = RT_SCOPE_HOST;
	else if (ifa->scope&IFA_LINK)
		ifm->ifa_scope = RT_SCOPE_LINK;
	else if (ifa->scope&IFA_SITE)
		ifm->ifa_scope = RT_SCOPE_SITE;
	ifm->ifa_index = ifa->idev->dev->ifindex;
	RTA_PUT(skb, IFA_ADDRESS, 16, &ifa->addr);
	if (!(ifa->flags&IFA_F_PERMANENT)) {
		ci.ifa_prefered = ifa->prefered_lft;
		ci.ifa_valid = ifa->valid_lft;
		if (ci.ifa_prefered != INFINITY_LIFE_TIME) {
			long tval = (jiffies - ifa->tstamp)/HZ;
			ci.ifa_prefered -= tval;
			if (ci.ifa_valid != INFINITY_LIFE_TIME)
				ci.ifa_valid -= tval;
		}
	} else {
		ci.ifa_prefered = INFINITY_LIFE_TIME;
		ci.ifa_valid = INFINITY_LIFE_TIME;
	}
	ci.cstamp = (__u32)(TIME_DELTA(ifa->cstamp, INITIAL_JIFFIES) / HZ * 100
		    + TIME_DELTA(ifa->cstamp, INITIAL_JIFFIES) % HZ * 100 / HZ);
	ci.tstamp = (__u32)(TIME_DELTA(ifa->tstamp, INITIAL_JIFFIES) / HZ * 100
		    + TIME_DELTA(ifa->tstamp, INITIAL_JIFFIES) % HZ * 100 / HZ);
	RTA_PUT(skb, IFA_CACHEINFO, sizeof(ci), &ci);
	nlh->nlmsg_len = skb->tail - b;
	return skb->len;

nlmsg_failure:
rtattr_failure:
	skb_trim(skb, b - skb->data);
	return -1;
}