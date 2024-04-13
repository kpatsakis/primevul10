static int pfkey_promisc(struct sock *sk, struct sk_buff *skb, const struct sadb_msg *hdr, void * const *ext_hdrs)
{
	struct pfkey_sock *pfk = pfkey_sk(sk);
	int satype = hdr->sadb_msg_satype;
	bool reset_errno = false;

	if (hdr->sadb_msg_len == (sizeof(*hdr) / sizeof(uint64_t))) {
		reset_errno = true;
		if (satype != 0 && satype != 1)
			return -EINVAL;
		pfk->promisc = satype;
	}
	if (reset_errno && skb_cloned(skb))
		skb = skb_copy(skb, GFP_KERNEL);
	else
		skb = skb_clone(skb, GFP_KERNEL);

	if (reset_errno && skb) {
		struct sadb_msg *new_hdr = (struct sadb_msg *) skb->data;
		new_hdr->sadb_msg_errno = 0;
	}

	pfkey_broadcast(skb, GFP_KERNEL, BROADCAST_ALL, NULL, sock_net(sk));
	return 0;
}