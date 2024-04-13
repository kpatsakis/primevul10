static struct mfc_cache *ipmr_mfc_seq_idx(struct ipmr_mfc_iter *it, loff_t pos)
{
	struct mfc_cache *mfc;

	it->cache = mfc_cache_array;
	read_lock(&mrt_lock);
	for (it->ct = 0; it->ct < MFC_LINES; it->ct++) 
		for(mfc = mfc_cache_array[it->ct]; mfc; mfc = mfc->next) 
			if (pos-- == 0) 
				return mfc;
	read_unlock(&mrt_lock);

	it->cache = &mfc_unres_queue;
	spin_lock_bh(&mfc_unres_lock);
	for(mfc = mfc_unres_queue; mfc; mfc = mfc->next) 
		if (pos-- == 0)
			return mfc;
	spin_unlock_bh(&mfc_unres_lock);

	it->cache = NULL;
	return NULL;
}