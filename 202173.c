static void *if6_seq_start(struct seq_file *seq, loff_t *pos)
{
	read_lock_bh(&addrconf_hash_lock);
	return if6_get_idx(seq, *pos);
}