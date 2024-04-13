static void ipmr_mfc_seq_stop(struct seq_file *seq, void *v)
{
	struct ipmr_mfc_iter *it = seq->private;

	if (it->cache == &mfc_unres_queue)
		spin_unlock_bh(&mfc_unres_lock);
	else if (it->cache == mfc_cache_array)
		read_unlock(&mrt_lock);
}