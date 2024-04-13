static void ipmr_vif_seq_stop(struct seq_file *seq, void *v)
{
	read_unlock(&mrt_lock);
}