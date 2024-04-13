static struct sctp_association *__sctp_endpoint_lookup_assoc(
	const struct sctp_endpoint *ep,
	const union sctp_addr *paddr,
	struct sctp_transport **transport)
{
	struct sctp_association *asoc = NULL;
	struct sctp_transport *t = NULL;
	struct sctp_hashbucket *head;
	struct sctp_ep_common *epb;
	struct hlist_node *node;
	int hash;
	int rport;

	*transport = NULL;
	rport = ntohs(paddr->v4.sin_port);

	hash = sctp_assoc_hashfn(ep->base.bind_addr.port, rport);
	head = &sctp_assoc_hashtable[hash];
	read_lock(&head->lock);
	sctp_for_each_hentry(epb, node, &head->chain) {
		asoc = sctp_assoc(epb);
		if (asoc->ep != ep || rport != asoc->peer.port)
			goto next;

		t = sctp_assoc_lookup_paddr(asoc, paddr);
		if (t) {
			*transport = t;
			break;
		}
next:
		asoc = NULL;
	}
	read_unlock(&head->lock);
	return asoc;
}