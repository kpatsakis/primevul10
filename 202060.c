static struct inet6_ifaddr *if6_get_idx(struct seq_file *seq, loff_t pos)
{
	struct inet6_ifaddr *ifa = if6_get_first(seq);

	if (ifa)
		while(pos && (ifa = if6_get_next(seq, ifa)) != NULL)
			--pos;
	return pos ? NULL : ifa;
}