static void *ipmr_vif_seq_next(struct seq_file *seq, void *v, loff_t *pos)
{
	struct ipmr_vif_iter *iter = seq->private;

	++*pos;
	if (v == SEQ_START_TOKEN)
		return ipmr_vif_seq_idx(iter, 0);
	
	while (++iter->ct < maxvif) {
		if(!VIF_EXISTS(iter->ct))
			continue;
		return &vif_table[iter->ct];
	}
	return NULL;
}