static int ipmr_mfc_delete(struct mfcctl *mfc)
{
	int line;
	struct mfc_cache *c, **cp;

	line=MFC_HASH(mfc->mfcc_mcastgrp.s_addr, mfc->mfcc_origin.s_addr);

	for (cp=&mfc_cache_array[line]; (c=*cp) != NULL; cp = &c->next) {
		if (c->mfc_origin == mfc->mfcc_origin.s_addr &&
		    c->mfc_mcastgrp == mfc->mfcc_mcastgrp.s_addr) {
			write_lock_bh(&mrt_lock);
			*cp = c->next;
			write_unlock_bh(&mrt_lock);

			kmem_cache_free(mrt_cachep, c);
			return 0;
		}
	}
	return -ENOENT;
}