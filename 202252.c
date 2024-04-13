int ipmr_get_route(struct sk_buff *skb, struct rtmsg *rtm, int nowait)
{
	int err;
	struct mfc_cache *cache;
	struct rtable *rt = (struct rtable*)skb->dst;

	read_lock(&mrt_lock);
	cache = ipmr_cache_find(rt->rt_src, rt->rt_dst);

	if (cache==NULL) {
		struct net_device *dev;
		int vif;

		if (nowait) {
			read_unlock(&mrt_lock);
			return -EAGAIN;
		}

		dev = skb->dev;
		if (dev == NULL || (vif = ipmr_find_vif(dev)) < 0) {
			read_unlock(&mrt_lock);
			return -ENODEV;
		}
		skb->nh.raw = skb_push(skb, sizeof(struct iphdr));
		skb->nh.iph->ihl = sizeof(struct iphdr)>>2;
		skb->nh.iph->saddr = rt->rt_src;
		skb->nh.iph->daddr = rt->rt_dst;
		skb->nh.iph->version = 0;
		err = ipmr_cache_unresolved(vif, skb);
		read_unlock(&mrt_lock);
		return err;
	}

	if (!nowait && (rtm->rtm_flags&RTM_F_NOTIFY))
		cache->mfc_flags |= MFC_NOTIFY;
	err = ipmr_fill_mroute(skb, cache, rtm);
	read_unlock(&mrt_lock);
	return err;
}