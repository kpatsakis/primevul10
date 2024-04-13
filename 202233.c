static int ip_mr_forward(struct sk_buff *skb, struct mfc_cache *cache, int local)
{
	int psend = -1;
	int vif, ct;

	vif = cache->mfc_parent;
	cache->mfc_un.res.pkt++;
	cache->mfc_un.res.bytes += skb->len;

	/*
	 * Wrong interface: drop packet and (maybe) send PIM assert.
	 */
	if (vif_table[vif].dev != skb->dev) {
		int true_vifi;

		if (((struct rtable*)skb->dst)->fl.iif == 0) {
			/* It is our own packet, looped back.
			   Very complicated situation...

			   The best workaround until routing daemons will be
			   fixed is not to redistribute packet, if it was
			   send through wrong interface. It means, that
			   multicast applications WILL NOT work for
			   (S,G), which have default multicast route pointing
			   to wrong oif. In any case, it is not a good
			   idea to use multicasting applications on router.
			 */
			goto dont_forward;
		}

		cache->mfc_un.res.wrong_if++;
		true_vifi = ipmr_find_vif(skb->dev);

		if (true_vifi >= 0 && mroute_do_assert &&
		    /* pimsm uses asserts, when switching from RPT to SPT,
		       so that we cannot check that packet arrived on an oif.
		       It is bad, but otherwise we would need to move pretty
		       large chunk of pimd to kernel. Ough... --ANK
		     */
		    (mroute_do_pim || cache->mfc_un.res.ttls[true_vifi] < 255) &&
		    time_after(jiffies, 
			       cache->mfc_un.res.last_assert + MFC_ASSERT_THRESH)) {
			cache->mfc_un.res.last_assert = jiffies;
			ipmr_cache_report(skb, true_vifi, IGMPMSG_WRONGVIF);
		}
		goto dont_forward;
	}

	vif_table[vif].pkt_in++;
	vif_table[vif].bytes_in+=skb->len;

	/*
	 *	Forward the frame
	 */
	for (ct = cache->mfc_un.res.maxvif-1; ct >= cache->mfc_un.res.minvif; ct--) {
		if (skb->nh.iph->ttl > cache->mfc_un.res.ttls[ct]) {
			if (psend != -1) {
				struct sk_buff *skb2 = skb_clone(skb, GFP_ATOMIC);
				if (skb2)
					ipmr_queue_xmit(skb2, cache, psend);
			}
			psend=ct;
		}
	}
	if (psend != -1) {
		if (local) {
			struct sk_buff *skb2 = skb_clone(skb, GFP_ATOMIC);
			if (skb2)
				ipmr_queue_xmit(skb2, cache, psend);
		} else {
			ipmr_queue_xmit(skb, cache, psend);
			return 0;
		}
	}

dont_forward:
	if (!local)
		kfree_skb(skb);
	return 0;
}