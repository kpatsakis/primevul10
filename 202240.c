static void ipmr_cache_resolve(struct mfc_cache *uc, struct mfc_cache *c)
{
	struct sk_buff *skb;
	struct nlmsgerr *e;

	/*
	 *	Play the pending entries through our router
	 */

	while((skb=__skb_dequeue(&uc->mfc_un.unres.unresolved))) {
		if (skb->nh.iph->version == 0) {
			int err;
			struct nlmsghdr *nlh = (struct nlmsghdr *)skb_pull(skb, sizeof(struct iphdr));

			if (ipmr_fill_mroute(skb, c, NLMSG_DATA(nlh)) > 0) {
				nlh->nlmsg_len = skb->tail - (u8*)nlh;
			} else {
				nlh->nlmsg_type = NLMSG_ERROR;
				nlh->nlmsg_len = NLMSG_LENGTH(sizeof(struct nlmsgerr));
				skb_trim(skb, nlh->nlmsg_len);
				e = NLMSG_DATA(nlh);
				e->error = -EMSGSIZE;
				memset(&e->msg, 0, sizeof(e->msg));
			}
			err = netlink_unicast(rtnl, skb, NETLINK_CB(skb).dst_pid, MSG_DONTWAIT);
		} else
			ip_mr_forward(skb, c, 0);
	}
}