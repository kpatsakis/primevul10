gnutls_priority_mac_list(gnutls_priority_t pcache,
			 const unsigned int **list)
{
	if (pcache->mac.algorithms == 0)
		return 0;

	*list = pcache->mac.priority;
	return pcache->mac.algorithms;
}