SCTP_STATIC void sctp_destroy_sock(struct sock *sk)
{
	struct sctp_endpoint *ep;

	SCTP_DEBUG_PRINTK("sctp_destroy_sock(sk: %p)\n", sk);

	/* Release our hold on the endpoint. */
	ep = sctp_sk(sk)->ep;
	sctp_endpoint_free(ep);
	atomic_dec(&sctp_sockets_allocated);
}