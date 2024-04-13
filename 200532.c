static int sctp_getsockopt_peer_addrs_old(struct sock *sk, int len,
					  char __user *optval,
					  int __user *optlen)
{
	struct sctp_association *asoc;
	int cnt = 0;
	struct sctp_getaddrs_old getaddrs;
	struct sctp_transport *from;
	void __user *to;
	union sctp_addr temp;
	struct sctp_sock *sp = sctp_sk(sk);
	int addrlen;

	if (len < sizeof(struct sctp_getaddrs_old))
		return -EINVAL;

	len = sizeof(struct sctp_getaddrs_old);

	if (copy_from_user(&getaddrs, optval, len))
		return -EFAULT;

	if (getaddrs.addr_num <= 0) return -EINVAL;

	printk(KERN_WARNING "SCTP: Use of SCTP_GET_PEER_ADDRS_OLD "
			    "socket option deprecated\n");

	/* For UDP-style sockets, id specifies the association to query.  */
	asoc = sctp_id2assoc(sk, getaddrs.assoc_id);
	if (!asoc)
		return -EINVAL;

	to = (void __user *)getaddrs.addrs;
	list_for_each_entry(from, &asoc->peer.transport_addr_list,
				transports) {
		memcpy(&temp, &from->ipaddr, sizeof(temp));
		sctp_get_pf_specific(sk->sk_family)->addr_v4map(sp, &temp);
		addrlen = sctp_get_af_specific(sk->sk_family)->sockaddr_len;
		if (copy_to_user(to, &temp, addrlen))
			return -EFAULT;
		to += addrlen ;
		cnt ++;
		if (cnt >= getaddrs.addr_num) break;
	}
	getaddrs.addr_num = cnt;
	if (put_user(len, optlen))
		return -EFAULT;
	if (copy_to_user(optval, &getaddrs, len))
		return -EFAULT;

	return 0;
}