static int pep_ioctl(struct sock *sk, int cmd, unsigned long arg)
{
	struct pep_sock *pn = pep_sk(sk);
	int answ;
	int ret = -ENOIOCTLCMD;

	switch (cmd) {
	case SIOCINQ:
		if (sk->sk_state == TCP_LISTEN) {
			ret = -EINVAL;
			break;
		}

		lock_sock(sk);
		if (sock_flag(sk, SOCK_URGINLINE) &&
		    !skb_queue_empty(&pn->ctrlreq_queue))
			answ = skb_peek(&pn->ctrlreq_queue)->len;
		else if (!skb_queue_empty(&sk->sk_receive_queue))
			answ = skb_peek(&sk->sk_receive_queue)->len;
		else
			answ = 0;
		release_sock(sk);
		ret = put_user(answ, (int __user *)arg);
		break;

	case SIOCPNENABLEPIPE:
		lock_sock(sk);
		if (sk->sk_state == TCP_SYN_SENT)
			ret =  -EBUSY;
		else if (sk->sk_state == TCP_ESTABLISHED)
			ret = -EISCONN;
		else
			ret = pep_sock_enable(sk, NULL, 0);
		release_sock(sk);
		break;
	}

	return ret;
}