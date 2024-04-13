static int ipmr_mfc_add(struct mfcctl *mfc, int mrtsock)
{
	int line;
	struct mfc_cache *uc, *c, **cp;

	line=MFC_HASH(mfc->mfcc_mcastgrp.s_addr, mfc->mfcc_origin.s_addr);

	for (cp=&mfc_cache_array[line]; (c=*cp) != NULL; cp = &c->next) {
		if (c->mfc_origin == mfc->mfcc_origin.s_addr &&
		    c->mfc_mcastgrp == mfc->mfcc_mcastgrp.s_addr)
			break;
	}

	if (c != NULL) {
		write_lock_bh(&mrt_lock);
		c->mfc_parent = mfc->mfcc_parent;
		ipmr_update_threshoulds(c, mfc->mfcc_ttls);
		if (!mrtsock)
			c->mfc_flags |= MFC_STATIC;
		write_unlock_bh(&mrt_lock);
		return 0;
	}

	if(!MULTICAST(mfc->mfcc_mcastgrp.s_addr))
		return -EINVAL;

	c=ipmr_cache_alloc();
	if (c==NULL)
		return -ENOMEM;

	c->mfc_origin=mfc->mfcc_origin.s_addr;
	c->mfc_mcastgrp=mfc->mfcc_mcastgrp.s_addr;
	c->mfc_parent=mfc->mfcc_parent;
	ipmr_update_threshoulds(c, mfc->mfcc_ttls);
	if (!mrtsock)
		c->mfc_flags |= MFC_STATIC;

	write_lock_bh(&mrt_lock);
	c->next = mfc_cache_array[line];
	mfc_cache_array[line] = c;
	write_unlock_bh(&mrt_lock);

	/*
	 *	Check to see if we resolved a queued list. If so we
	 *	need to send on the frames and tidy up.
	 */
	spin_lock_bh(&mfc_unres_lock);
	for (cp = &mfc_unres_queue; (uc=*cp) != NULL;
	     cp = &uc->next) {
		if (uc->mfc_origin == c->mfc_origin &&
		    uc->mfc_mcastgrp == c->mfc_mcastgrp) {
			*cp = uc->next;
			if (atomic_dec_and_test(&cache_resolve_queue_len))
				del_timer(&ipmr_expire_timer);
			break;
		}
	}
	spin_unlock_bh(&mfc_unres_lock);

	if (uc) {
		ipmr_cache_resolve(uc, c);
		kmem_cache_free(mrt_cachep, uc);
	}
	return 0;
}