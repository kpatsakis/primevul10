static void *ipmr_mfc_seq_next(struct seq_file *seq, void *v, loff_t *pos)
{
	struct mfc_cache *mfc = v;
	struct ipmr_mfc_iter *it = seq->private;

	++*pos;

	if (v == SEQ_START_TOKEN)
		return ipmr_mfc_seq_idx(seq->private, 0);

	if (mfc->next)
		return mfc->next;
	
	if (it->cache == &mfc_unres_queue) 
		goto end_of_list;

	BUG_ON(it->cache != mfc_cache_array);

	while (++it->ct < MFC_LINES) {
		mfc = mfc_cache_array[it->ct];
		if (mfc)
			return mfc;
	}

	/* exhausted cache_array, show unresolved */
	read_unlock(&mrt_lock);
	it->cache = &mfc_unres_queue;
	it->ct = 0;
		
	spin_lock_bh(&mfc_unres_lock);
	mfc = mfc_unres_queue;
	if (mfc) 
		return mfc;

 end_of_list:
	spin_unlock_bh(&mfc_unres_lock);
	it->cache = NULL;

	return NULL;
}