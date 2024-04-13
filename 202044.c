static void if6_seq_stop(struct seq_file *seq, void *v)
{
	read_unlock_bh(&addrconf_hash_lock);
}