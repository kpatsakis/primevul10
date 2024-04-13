gnutls_priority_ecc_curve_list(gnutls_priority_t pcache,
			       const unsigned int **list)
{
	if (pcache->supported_ecc.algorithms == 0)
		return 0;

	*list = pcache->supported_ecc.priority;
	return pcache->supported_ecc.algorithms;
}