static struct inet6_ifaddr *if6_get_next(struct seq_file *seq, struct inet6_ifaddr *ifa)
{
	struct if6_iter_state *state = seq->private;

	ifa = ifa->lst_next;
try_again:
	if (!ifa && ++state->bucket < IN6_ADDR_HSIZE) {
		ifa = inet6_addr_lst[state->bucket];
		goto try_again;
	}
	return ifa;
}