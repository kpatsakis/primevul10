gnutls_priority_certificate_type_list(gnutls_priority_t pcache,
				      const unsigned int **list)
{
	if (pcache->cert_type.algorithms == 0)
		return 0;

	*list = pcache->cert_type.priority;
	return pcache->cert_type.algorithms;
}