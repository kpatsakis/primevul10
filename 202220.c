static void *ipmr_vif_seq_start(struct seq_file *seq, loff_t *pos)
{
	read_lock(&mrt_lock);
	return *pos ? ipmr_vif_seq_idx(seq->private, *pos - 1) 
		: SEQ_START_TOKEN;
}