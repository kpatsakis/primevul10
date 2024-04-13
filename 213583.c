ff_layout_encode_netaddr(struct xdr_stream *xdr, struct nfs4_pnfs_ds_addr *da)
{
	struct sockaddr *sap = (struct sockaddr *)&da->da_addr;
	char portbuf[RPCBIND_MAXUADDRPLEN];
	char addrbuf[RPCBIND_MAXUADDRLEN];
	unsigned short port;
	int len, netid_len;
	__be32 *p;

	switch (sap->sa_family) {
	case AF_INET:
		if (ff_layout_ntop4(sap, addrbuf, sizeof(addrbuf)) == 0)
			return;
		port = ntohs(((struct sockaddr_in *)sap)->sin_port);
		break;
	case AF_INET6:
		if (ff_layout_ntop6_noscopeid(sap, addrbuf, sizeof(addrbuf)) == 0)
			return;
		port = ntohs(((struct sockaddr_in6 *)sap)->sin6_port);
		break;
	default:
		WARN_ON_ONCE(1);
		return;
	}

	snprintf(portbuf, sizeof(portbuf), ".%u.%u", port >> 8, port & 0xff);
	len = strlcat(addrbuf, portbuf, sizeof(addrbuf));

	netid_len = strlen(da->da_netid);
	p = xdr_reserve_space(xdr, 4 + netid_len);
	xdr_encode_opaque(p, da->da_netid, netid_len);

	p = xdr_reserve_space(xdr, 4 + len);
	xdr_encode_opaque(p, addrbuf, len);
}