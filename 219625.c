server_find_pk_algos_in_ciphersuites(const uint8_t *
				     data, unsigned int datalen,
				     gnutls_pk_algorithm_t * algos,
				     size_t * algos_size)
{
	unsigned int j, x;
	gnutls_kx_algorithm_t kx;
	gnutls_pk_algorithm_t pk;
	unsigned found;
	unsigned int max = *algos_size;

	if (datalen % 2 != 0) {
		gnutls_assert();
		return GNUTLS_E_UNEXPECTED_PACKET_LENGTH;
	}

	*algos_size = 0;
	for (j = 0; j < datalen; j += 2) {
		kx = _gnutls_cipher_suite_get_kx_algo(&data[j]);
		if (_gnutls_map_kx_get_cred(kx, 1) ==
		    GNUTLS_CRD_CERTIFICATE) {
			pk = _gnutls_map_pk_get_pk(kx);
			found = 0;
			for (x = 0; x < *algos_size; x++) {
				if (algos[x] == pk) {
					found = 1;
					break;
				}
			}

			if (found == 0) {
				algos[(*algos_size)++] =
				    _gnutls_map_pk_get_pk(kx);
				if ((*algos_size) >= max)
					return 0;
			}
		}
	}

	return 0;
}