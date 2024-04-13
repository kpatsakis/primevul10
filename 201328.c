void selinux_netlbl_sk_security_reset(struct sk_security_struct *ssec,
				      int family)
{
	if (family == PF_INET)
		ssec->nlbl_state = NLBL_REQUIRE;
	else
		ssec->nlbl_state = NLBL_UNSET;
}