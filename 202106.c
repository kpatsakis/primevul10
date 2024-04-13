static int inline ipv6_saddr_pref(const struct inet6_ifaddr *ifp, u8 invpref)
{
	int pref;
	pref = ifp->flags&IFA_F_DEPRECATED ? 0 : 2;
#ifdef CONFIG_IPV6_PRIVACY
	pref |= (ifp->flags^invpref)&IFA_F_TEMPORARY ? 0 : 1;
#endif
	return pref;
}