static int verify_address_len(const void *p)
{
	const struct sadb_address *sp = p;
	const struct sockaddr *addr = (const struct sockaddr *)(sp + 1);
	const struct sockaddr_in *sin;
#if IS_ENABLED(CONFIG_IPV6)
	const struct sockaddr_in6 *sin6;
#endif
	int len;

	if (sp->sadb_address_len <
	    DIV_ROUND_UP(sizeof(*sp) + offsetofend(typeof(*addr), sa_family),
			 sizeof(uint64_t)))
		return -EINVAL;

	switch (addr->sa_family) {
	case AF_INET:
		len = DIV_ROUND_UP(sizeof(*sp) + sizeof(*sin), sizeof(uint64_t));
		if (sp->sadb_address_len != len ||
		    sp->sadb_address_prefixlen > 32)
			return -EINVAL;
		break;
#if IS_ENABLED(CONFIG_IPV6)
	case AF_INET6:
		len = DIV_ROUND_UP(sizeof(*sp) + sizeof(*sin6), sizeof(uint64_t));
		if (sp->sadb_address_len != len ||
		    sp->sadb_address_prefixlen > 128)
			return -EINVAL;
		break;
#endif
	default:
		/* It is user using kernel to keep track of security
		 * associations for another protocol, such as
		 * OSPF/RSVP/RIPV2/MIP.  It is user's job to verify
		 * lengths.
		 *
		 * XXX Actually, association/policy database is not yet
		 * XXX able to cope with arbitrary sockaddr families.
		 * XXX When it can, remove this -EINVAL.  -DaveM
		 */
		return -EINVAL;
	}

	return 0;
}