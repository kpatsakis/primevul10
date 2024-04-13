gnutls_priority_cipher_list(gnutls_priority_t pcache,
			    const unsigned int **list)
{
	if (pcache->cipher.algorithms == 0)
		return 0;

	*list = pcache->cipher.priority;
	return pcache->cipher.algorithms;
}