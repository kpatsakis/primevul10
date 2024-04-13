static struct netlbl_lsm_secattr *selinux_netlbl_sock_genattr(struct sock *sk)
{
	int rc;
	struct sk_security_struct *sksec = sk->sk_security;
	struct netlbl_lsm_secattr *secattr;

	if (sksec->nlbl_secattr != NULL)
		return sksec->nlbl_secattr;

	secattr = netlbl_secattr_alloc(GFP_ATOMIC);
	if (secattr == NULL)
		return NULL;
	rc = security_netlbl_sid_to_secattr(sksec->sid, secattr);
	if (rc != 0) {
		netlbl_secattr_free(secattr);
		return NULL;
	}
	sksec->nlbl_secattr = secattr;

	return secattr;
}