gnutls_priority_protocol_list(gnutls_priority_t pcache,
			      const unsigned int **list)
{
	if (pcache->protocol.algorithms == 0)
		return 0;

	*list = pcache->protocol.priority;
	return pcache->protocol.algorithms;
}