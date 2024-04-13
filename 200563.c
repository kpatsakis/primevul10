static int sctp_getsockopt_local_addrs_num_old(struct sock *sk, int len,
					       char __user *optval,
					       int __user *optlen)
{
	sctp_assoc_t id;
	struct sctp_bind_addr *bp;
	struct sctp_association *asoc;
	struct sctp_sockaddr_entry *addr;
	int cnt = 0;

	if (len < sizeof(sctp_assoc_t))
		return -EINVAL;

	if (copy_from_user(&id, optval, sizeof(sctp_assoc_t)))
		return -EFAULT;

	printk(KERN_WARNING "SCTP: Use of SCTP_GET_LOCAL_ADDRS_NUM_OLD "
			    "socket option deprecated\n");

	/*
	 *  For UDP-style sockets, id specifies the association to query.
	 *  If the id field is set to the value '0' then the locally bound
	 *  addresses are returned without regard to any particular
	 *  association.
	 */
	if (0 == id) {
		bp = &sctp_sk(sk)->ep->base.bind_addr;
	} else {
		asoc = sctp_id2assoc(sk, id);
		if (!asoc)
			return -EINVAL;
		bp = &asoc->base.bind_addr;
	}

	/* If the endpoint is bound to 0.0.0.0 or ::0, count the valid
	 * addresses from the global local address list.
	 */
	if (sctp_list_single_entry(&bp->address_list)) {
		addr = list_entry(bp->address_list.next,
				  struct sctp_sockaddr_entry, list);
		if (sctp_is_any(&addr->a)) {
			rcu_read_lock();
			list_for_each_entry_rcu(addr,
						&sctp_local_addr_list, list) {
				if (!addr->valid)
					continue;

				if ((PF_INET == sk->sk_family) &&
				    (AF_INET6 == addr->a.sa.sa_family))
					continue;

				if ((PF_INET6 == sk->sk_family) &&
				    inet_v6_ipv6only(sk) &&
				    (AF_INET == addr->a.sa.sa_family))
					continue;

				cnt++;
			}
			rcu_read_unlock();
		} else {
			cnt = 1;
		}
		goto done;
	}

	/* Protection on the bound address list is not needed,
	 * since in the socket option context we hold the socket lock,
	 * so there is no way that the bound address list can change.
	 */
	list_for_each_entry(addr, &bp->address_list, list) {
		cnt ++;
	}
done:
	return cnt;
}