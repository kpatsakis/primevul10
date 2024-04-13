static int rose_accept(struct socket *sock, struct socket *newsock, int flags)
{
	struct sk_buff *skb;
	struct sock *newsk;
	DEFINE_WAIT(wait);
	struct sock *sk;
	int err = 0;

	if ((sk = sock->sk) == NULL)
		return -EINVAL;

	lock_sock(sk);
	if (sk->sk_type != SOCK_SEQPACKET) {
		err = -EOPNOTSUPP;
		goto out_release;
	}

	if (sk->sk_state != TCP_LISTEN) {
		err = -EINVAL;
		goto out_release;
	}

	/*
	 *	The write queue this time is holding sockets ready to use
	 *	hooked into the SABM we saved
	 */
	for (;;) {
		prepare_to_wait(sk->sk_sleep, &wait, TASK_INTERRUPTIBLE);

		skb = skb_dequeue(&sk->sk_receive_queue);
		if (skb)
			break;

		if (flags & O_NONBLOCK) {
			err = -EWOULDBLOCK;
			break;
		}
		if (!signal_pending(current)) {
			release_sock(sk);
			schedule();
			lock_sock(sk);
			continue;
		}
		err = -ERESTARTSYS;
		break;
	}
	finish_wait(sk->sk_sleep, &wait);
	if (err)
		goto out_release;

	newsk = skb->sk;
	sock_graft(newsk, newsock);

	/* Now attach up the new socket */
	skb->sk = NULL;
	kfree_skb(skb);
	sk->sk_ack_backlog--;

out_release:
	release_sock(sk);

	return err;
}