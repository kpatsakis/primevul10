gnutls_priority_kx_list(gnutls_priority_t pcache,
			const unsigned int **list)
{
	if (pcache->kx.algorithms == 0)
		return 0;

	*list = pcache->kx.priority;
	return pcache->kx.algorithms;
}