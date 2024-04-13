static int create_tls_random(uint8_t * dst)
{
	uint32_t tim;
	int ret;

	/* Use weak random numbers for the most of the
	 * buffer except for the first 4 that are the
	 * system's time.
	 */

	tim = gnutls_time(NULL);
	/* generate server random value */
	_gnutls_write_uint32(tim, dst);

	ret =
	    _gnutls_rnd(GNUTLS_RND_NONCE, &dst[3], GNUTLS_RANDOM_SIZE - 3);
	if (ret < 0) {
		gnutls_assert();
		return ret;
	}

	return 0;
}