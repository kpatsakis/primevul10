void selinux_netlbl_inet_conn_established(struct sock *sk, u16 family)
{
	int rc;
	struct sk_security_struct *sksec = sk->sk_security;
	struct netlbl_lsm_secattr *secattr;
	struct inet_sock *sk_inet = inet_sk(sk);
	struct sockaddr_in addr;

	if (sksec->nlbl_state != NLBL_REQUIRE)
		return;

	secattr = selinux_netlbl_sock_genattr(sk);
	if (secattr == NULL)
		return;

	rc = netlbl_sock_setattr(sk, secattr);
	switch (rc) {
	case 0:
		sksec->nlbl_state = NLBL_LABELED;
		break;
	case -EDESTADDRREQ:
		/* no PF_INET6 support yet because we don't support any IPv6
		 * labeling protocols */
		if (family != PF_INET) {
			sksec->nlbl_state = NLBL_UNSET;
			return;
		}

		addr.sin_family = family;
		addr.sin_addr.s_addr = sk_inet->daddr;
		if (netlbl_conn_setattr(sk, (struct sockaddr *)&addr,
					secattr) != 0) {
			/* we failed to label the connected socket (could be
			 * for a variety of reasons, the actual "why" isn't
			 * important here) so we have to go to our backup plan,
			 * labeling the packets individually in the netfilter
			 * local output hook.  this is okay but we need to
			 * adjust the MSS of the connection to take into
			 * account any labeling overhead, since we don't know
			 * the exact overhead at this point we'll use the worst
			 * case value which is 40 bytes for IPv4 */
			struct inet_connection_sock *sk_conn = inet_csk(sk);
			sk_conn->icsk_ext_hdr_len += 40 -
				      (sk_inet->opt ? sk_inet->opt->optlen : 0);
			sk_conn->icsk_sync_mss(sk, sk_conn->icsk_pmtu_cookie);

			sksec->nlbl_state = NLBL_REQSKB;
		} else
			sksec->nlbl_state = NLBL_CONNLABELED;
		break;
	default:
		/* note that we are failing to label the socket which could be
		 * a bad thing since it means traffic could leave the system
		 * without the desired labeling, however, all is not lost as
		 * we have a check in selinux_netlbl_inode_permission() to
		 * pick up the pieces that we might drop here because we can't
		 * return an error code */
		break;
	}
}