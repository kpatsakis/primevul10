gnutls_priority_sign_list(gnutls_priority_t pcache,
			  const unsigned int **list)
{
	if (pcache->sign_algo.algorithms == 0)
		return 0;

	*list = pcache->sign_algo.priority;
	return pcache->sign_algo.algorithms;
}