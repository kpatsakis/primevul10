void selinux_netlbl_sk_security_free(struct sk_security_struct *ssec)
{
	if (ssec->nlbl_secattr != NULL)
		netlbl_secattr_free(ssec->nlbl_secattr);
}