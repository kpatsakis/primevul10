static struct inet6_ifaddr *if6_get_first(struct seq_file *seq)
{
	struct inet6_ifaddr *ifa = NULL;
	struct if6_iter_state *state = seq->private;

	for (state->bucket = 0; state->bucket < IN6_ADDR_HSIZE; ++state->bucket) {
		ifa = inet6_addr_lst[state->bucket];
		if (ifa)
			break;
	}
	return ifa;
}