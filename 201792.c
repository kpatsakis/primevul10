static struct sock *udp_get_first(struct seq_file *seq)
{
	struct sock *sk;
	struct udp_iter_state *state = seq->private;

	for (state->bucket = 0; state->bucket < UDP_HTABLE_SIZE; ++state->bucket) {
		struct hlist_node *node;
		sk_for_each(sk, node, &udp_hash[state->bucket]) {
			if (sk->sk_family == state->family)
				goto found;
		}
	}
	sk = NULL;
found:
	return sk;
}