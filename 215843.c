static int pfkey_sockaddr_pair_size(sa_family_t family)
{
	return PFKEY_ALIGN8(pfkey_sockaddr_len(family) * 2);
}