ipmr_cache_unresolved(vifi_t vifi, struct sk_buff *skb)
{
	int err;
	struct mfc_cache *c;

	spin_lock_bh(&mfc_unres_lock);
	for (c=mfc_unres_queue; c; c=c->next) {
		if (c->mfc_mcastgrp == skb->nh.iph->daddr &&
		    c->mfc_origin == skb->nh.iph->saddr)
			break;
	}

	if (c == NULL) {
		/*
		 *	Create a new entry if allowable
		 */

		if (atomic_read(&cache_resolve_queue_len)>=10 ||
		    (c=ipmr_cache_alloc_unres())==NULL) {
			spin_unlock_bh(&mfc_unres_lock);

			kfree_skb(skb);
			return -ENOBUFS;
		}

		/*
		 *	Fill in the new cache entry
		 */
		c->mfc_parent=-1;
		c->mfc_origin=skb->nh.iph->saddr;
		c->mfc_mcastgrp=skb->nh.iph->daddr;

		/*
		 *	Reflect first query at mrouted.
		 */
		if ((err = ipmr_cache_report(skb, vifi, IGMPMSG_NOCACHE))<0) {
			/* If the report failed throw the cache entry 
			   out - Brad Parker
			 */
			spin_unlock_bh(&mfc_unres_lock);

			kmem_cache_free(mrt_cachep, c);
			kfree_skb(skb);
			return err;
		}

		atomic_inc(&cache_resolve_queue_len);
		c->next = mfc_unres_queue;
		mfc_unres_queue = c;

		mod_timer(&ipmr_expire_timer, c->mfc_un.unres.expires);
	}

	/*
	 *	See if we can append the packet
	 */
	if (c->mfc_un.unres.unresolved.qlen>3) {
		kfree_skb(skb);
		err = -ENOBUFS;
	} else {
		skb_queue_tail(&c->mfc_un.unres.unresolved,skb);
		err = 0;
	}

	spin_unlock_bh(&mfc_unres_lock);
	return err;
}