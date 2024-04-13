static void mroute_clean_tables(struct sock *sk)
{
	int i;
		
	/*
	 *	Shut down all active vif entries
	 */
	for(i=0; i<maxvif; i++) {
		if (!(vif_table[i].flags&VIFF_STATIC))
			vif_delete(i);
	}

	/*
	 *	Wipe the cache
	 */
	for (i=0;i<MFC_LINES;i++) {
		struct mfc_cache *c, **cp;

		cp = &mfc_cache_array[i];
		while ((c = *cp) != NULL) {
			if (c->mfc_flags&MFC_STATIC) {
				cp = &c->next;
				continue;
			}
			write_lock_bh(&mrt_lock);
			*cp = c->next;
			write_unlock_bh(&mrt_lock);

			kmem_cache_free(mrt_cachep, c);
		}
	}

	if (atomic_read(&cache_resolve_queue_len) != 0) {
		struct mfc_cache *c;

		spin_lock_bh(&mfc_unres_lock);
		while (mfc_unres_queue != NULL) {
			c = mfc_unres_queue;
			mfc_unres_queue = c->next;
			spin_unlock_bh(&mfc_unres_lock);

			ipmr_destroy_unres(c);

			spin_lock_bh(&mfc_unres_lock);
		}
		spin_unlock_bh(&mfc_unres_lock);
	}
}