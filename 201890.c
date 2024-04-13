	__acquires(rose_list_lock)
{
	int i;
	struct sock *s;
	struct hlist_node *node;

	spin_lock_bh(&rose_list_lock);
	if (*pos == 0)
		return SEQ_START_TOKEN;

	i = 1;
	sk_for_each(s, node, &rose_list) {
		if (i == *pos)
			return s;
		++i;
	}
	return NULL;
}