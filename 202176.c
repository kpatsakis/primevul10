static int if6_seq_show(struct seq_file *seq, void *v)
{
	struct inet6_ifaddr *ifp = (struct inet6_ifaddr *)v;
	seq_printf(seq,
		   "%04x%04x%04x%04x%04x%04x%04x%04x %02x %02x %02x %02x %8s\n",
		   NIP6(ifp->addr),
		   ifp->idev->dev->ifindex,
		   ifp->prefix_len,
		   ifp->scope,
		   ifp->flags,
		   ifp->idev->dev->name);
	return 0;
}