static void rtnetlink_rcv(struct sock *sk, int len)
{
	unsigned int qlen = skb_queue_len(&sk->sk_receive_queue);

	do {
		struct sk_buff *skb;

		rtnl_lock();

		if (qlen > skb_queue_len(&sk->sk_receive_queue))
			qlen = skb_queue_len(&sk->sk_receive_queue);

		for (; qlen; qlen--) {
			skb = skb_dequeue(&sk->sk_receive_queue);
			if (rtnetlink_rcv_skb(skb)) {
				if (skb->len)
					skb_queue_head(&sk->sk_receive_queue,
						       skb);
				else {
					kfree_skb(skb);
					qlen--;
				}
				break;
			}
			kfree_skb(skb);
		}

		up(&rtnl_sem);

		netdev_run_todo();
	} while (qlen);
}