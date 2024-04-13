static int inet6_dump_addr(struct sk_buff *skb, struct netlink_callback *cb,
			   enum addr_type_t type)
{
	int idx, ip_idx;
	int s_idx, s_ip_idx;
	int err = 1;
	struct net_device *dev;
	struct inet6_dev *idev = NULL;
	struct inet6_ifaddr *ifa;
	struct ifmcaddr6 *ifmca;
	struct ifacaddr6 *ifaca;

	s_idx = cb->args[0];
	s_ip_idx = ip_idx = cb->args[1];
	read_lock(&dev_base_lock);
	
	for (dev = dev_base, idx = 0; dev; dev = dev->next, idx++) {
		if (idx < s_idx)
			continue;
		if (idx > s_idx)
			s_ip_idx = 0;
		ip_idx = 0;
		if ((idev = in6_dev_get(dev)) == NULL)
			continue;
		read_lock_bh(&idev->lock);
		switch (type) {
		case UNICAST_ADDR:
			/* unicast address */
			for (ifa = idev->addr_list; ifa;
			     ifa = ifa->if_next, ip_idx++) {
				if (ip_idx < s_ip_idx)
					continue;
				if ((err = inet6_fill_ifaddr(skb, ifa, 
				    NETLINK_CB(cb->skb).pid, 
				    cb->nlh->nlmsg_seq, RTM_NEWADDR,
				    NLM_F_MULTI)) <= 0)
					goto done;
			}
			/* temp addr */
#ifdef CONFIG_IPV6_PRIVACY
			for (ifa = idev->tempaddr_list; ifa; 
			     ifa = ifa->tmp_next, ip_idx++) {
				if (ip_idx < s_ip_idx)
					continue;
				if ((err = inet6_fill_ifaddr(skb, ifa, 
				    NETLINK_CB(cb->skb).pid, 
				    cb->nlh->nlmsg_seq, RTM_NEWADDR,
				    NLM_F_MULTI)) <= 0) 
					goto done;
			}
#endif
			break;
		case MULTICAST_ADDR:
			/* multicast address */
			for (ifmca = idev->mc_list; ifmca; 
			     ifmca = ifmca->next, ip_idx++) {
				if (ip_idx < s_ip_idx)
					continue;
				if ((err = inet6_fill_ifmcaddr(skb, ifmca, 
				    NETLINK_CB(cb->skb).pid, 
				    cb->nlh->nlmsg_seq, RTM_GETMULTICAST,
				    NLM_F_MULTI)) <= 0)
					goto done;
			}
			break;
		case ANYCAST_ADDR:
			/* anycast address */
			for (ifaca = idev->ac_list; ifaca;
			     ifaca = ifaca->aca_next, ip_idx++) {
				if (ip_idx < s_ip_idx)
					continue;
				if ((err = inet6_fill_ifacaddr(skb, ifaca, 
				    NETLINK_CB(cb->skb).pid, 
				    cb->nlh->nlmsg_seq, RTM_GETANYCAST,
				    NLM_F_MULTI)) <= 0) 
					goto done;
			}
			break;
		default:
			break;
		}
		read_unlock_bh(&idev->lock);
		in6_dev_put(idev);
	}
done:
	if (err <= 0) {
		read_unlock_bh(&idev->lock);
		in6_dev_put(idev);
	}
	read_unlock(&dev_base_lock);
	cb->args[0] = idx;
	cb->args[1] = ip_idx;
	return skb->len;
}