static void *ipmr_mfc_seq_start(struct seq_file *seq, loff_t *pos)
{
	struct ipmr_mfc_iter *it = seq->private;
	it->cache = NULL;
	it->ct = 0;
	return *pos ? ipmr_mfc_seq_idx(seq->private, *pos - 1) 
		: SEQ_START_TOKEN;
}