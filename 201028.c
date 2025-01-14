void sock_rfree(struct sk_buff *skb)
{
	struct sock *sk = skb->sk;

	skb_truesize_check(skb);
	atomic_sub(skb->truesize, &sk->sk_rmem_alloc);
	sk_mem_uncharge(skb->sk, skb->truesize);
}