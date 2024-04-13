static int inet6_fill_ifmcaddr(struct sk_buff *skb, struct ifmcaddr6 *ifmca,
				u32 pid, u32 seq, int event, u16 flags)
{
	struct ifaddrmsg *ifm;
	struct nlmsghdr  *nlh;
	struct ifa_cacheinfo ci;
	unsigned char	 *b = skb->tail;

	nlh = NLMSG_NEW(skb, pid, seq, event, sizeof(*ifm), flags);
	ifm = NLMSG_DATA(nlh);
	ifm->ifa_family = AF_INET6;	
	ifm->ifa_prefixlen = 128;
	ifm->ifa_flags = IFA_F_PERMANENT;
	ifm->ifa_scope = RT_SCOPE_UNIVERSE;
	if (ipv6_addr_scope(&ifmca->mca_addr)&IFA_SITE)
		ifm->ifa_scope = RT_SCOPE_SITE;
	ifm->ifa_index = ifmca->idev->dev->ifindex;
	RTA_PUT(skb, IFA_MULTICAST, 16, &ifmca->mca_addr);
	ci.cstamp = (__u32)(TIME_DELTA(ifmca->mca_cstamp, INITIAL_JIFFIES) / HZ
		    * 100 + TIME_DELTA(ifmca->mca_cstamp, INITIAL_JIFFIES) % HZ
		    * 100 / HZ);
	ci.tstamp = (__u32)(TIME_DELTA(ifmca->mca_tstamp, INITIAL_JIFFIES) / HZ
		    * 100 + TIME_DELTA(ifmca->mca_tstamp, INITIAL_JIFFIES) % HZ
		    * 100 / HZ);
	ci.ifa_prefered = INFINITY_LIFE_TIME;
	ci.ifa_valid = INFINITY_LIFE_TIME;
	RTA_PUT(skb, IFA_CACHEINFO, sizeof(ci), &ci);
	nlh->nlmsg_len = skb->tail - b;
	return skb->len;

nlmsg_failure:
rtattr_failure:
	skb_trim(skb, b - skb->data);
	return -1;
}