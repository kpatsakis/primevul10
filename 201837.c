static void udpv6_close(struct sock *sk, long timeout)
{
	sk_common_release(sk);
}