static int sctp_setsockopt_partial_delivery_point(struct sock *sk,
						  char __user *optval,
						  int optlen)
{
	u32 val;

	if (optlen != sizeof(u32))
		return -EINVAL;
	if (get_user(val, (int __user *)optval))
		return -EFAULT;

	sctp_sk(sk)->pd_point = val;

	return 0; /* is this the right error code? */
}