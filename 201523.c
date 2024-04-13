static void tun_sock_write_space(struct sock *sk)
{
	struct tun_struct *tun;

	if (!sock_writeable(sk))
		return;

	if (!test_and_clear_bit(SOCK_ASYNC_NOSPACE, &sk->sk_socket->flags))
		return;

	if (sk->sk_sleep && waitqueue_active(sk->sk_sleep))
		wake_up_interruptible_sync(sk->sk_sleep);

	tun = container_of(sk, struct tun_sock, sk)->tun;
	kill_fasync(&tun->fasync, SIGIO, POLL_OUT);
}