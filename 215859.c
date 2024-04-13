static int pfkey_broadcast_one(struct sk_buff *skb, gfp_t allocation,
			       struct sock *sk)
{
	int err = -ENOBUFS;

	if (atomic_read(&sk->sk_rmem_alloc) > sk->sk_rcvbuf)
		return err;

	skb = skb_clone(skb, allocation);

	if (skb) {
		skb_set_owner_r(skb, sk);
		skb_queue_tail(&sk->sk_receive_queue, skb);
		sk->sk_data_ready(sk);
		err = 0;
	}
	return err;
}