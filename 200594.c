static int sctp_copy_laddrs_old(struct sock *sk, __u16 port,
					int max_addrs, void *to,
					int *bytes_copied)
{
	struct sctp_sockaddr_entry *addr;
	union sctp_addr temp;
	int cnt = 0;
	int addrlen;

	rcu_read_lock();
	list_for_each_entry_rcu(addr, &sctp_local_addr_list, list) {
		if (!addr->valid)
			continue;

		if ((PF_INET == sk->sk_family) &&
		    (AF_INET6 == addr->a.sa.sa_family))
			continue;
		if ((PF_INET6 == sk->sk_family) &&
		    inet_v6_ipv6only(sk) &&
		    (AF_INET == addr->a.sa.sa_family))
			continue;
		memcpy(&temp, &addr->a, sizeof(temp));
		if (!temp.v4.sin_port)
			temp.v4.sin_port = htons(port);

		sctp_get_pf_specific(sk->sk_family)->addr_v4map(sctp_sk(sk),
								&temp);
		addrlen = sctp_get_af_specific(temp.sa.sa_family)->sockaddr_len;
		memcpy(to, &temp, addrlen);

		to += addrlen;
		*bytes_copied += addrlen;
		cnt ++;
		if (cnt >= max_addrs) break;
	}
	rcu_read_unlock();

	return cnt;
}