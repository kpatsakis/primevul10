gnutls_priority_compression_list(gnutls_priority_t pcache,
				 const unsigned int **list)
{
	if (pcache->compression.algorithms == 0)
		return 0;

	*list = pcache->compression.priority;
	return pcache->compression.algorithms;
}