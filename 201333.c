static int selinux_netlbl_sock_setsid(struct sock *sk)
{
	int rc;
	struct sk_security_struct *sksec = sk->sk_security;
	struct netlbl_lsm_secattr *secattr;

	if (sksec->nlbl_state != NLBL_REQUIRE)
		return 0;

	secattr = selinux_netlbl_sock_genattr(sk);
	if (secattr == NULL)
		return -ENOMEM;
	rc = netlbl_sock_setattr(sk, secattr);
	switch (rc) {
	case 0:
		sksec->nlbl_state = NLBL_LABELED;
		break;
	case -EDESTADDRREQ:
		sksec->nlbl_state = NLBL_REQSKB;
		rc = 0;
		break;
	}

	return rc;
}