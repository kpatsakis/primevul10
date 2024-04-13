cardos_compute_signature(sc_card_t *card, const u8 *data, size_t datalen,
			 u8 *out, size_t outlen)
{
	cardos_data_t* priv;
	int    r;
	sc_context_t *ctx;
	int do_rsa_pure_sig = 0;
	int do_rsa_sig = 0;
	size_t i;


	assert(card != NULL && data != NULL && out != NULL);
	ctx = card->ctx;
	priv = (cardos_data_t*)card->drv_data;
	SC_FUNC_CALLED(ctx, SC_LOG_DEBUG_VERBOSE);

	/* sec_env has algorithm_flags set from sc_get_encoding_flags sec_flags
	 * If flags are set correctly we don't need to test anything 
	 * TODO this assumes RSA is  PSS, PKCS1 or RAW and we are passing 
	 * the correct data. Should work for ECDSA too.    
	 * use for V5 cards and TODO should for older cards too
	 */
	if (card->type == SC_CARD_TYPE_CARDOS_V5_0 || card->type == SC_CARD_TYPE_CARDOS_V5_3) {

		r = do_compute_signature(card, data, datalen, out, outlen);
		LOG_FUNC_RETURN(ctx, r);
	}

	/* There are two ways to create a signature, depending on the way,
	 * the key was created: RSA_SIG and RSA_PURE_SIG.
	 * We can use the following reasoning, to determine the correct operation:
	 * 1. We check for several caps flags (as set in card->caps), to prevent generating
	 *    invalid signatures with duplicated hash prefixes with some cards
	 * 2. Use the information from AlgorithmInfo of the TokenInfo file.
	 *    This information is parsed in set_security_env and stored in a static variable.
	 *    The problem is, that that information is only available for the whole token and not
	      for a specific key, so if both operations are present, we can only do trial and error
	 *
	 * The Algorithm IDs for RSA_SIG are 0x86 and 0x88, those for RSA_PURE_SIG 0x8c and 0x8a
	 * (According to http://www.opensc-project.org/pipermail/opensc-devel/2010-September/014912.html
	 *   and www.crysys.hu/infsec/M40_Manual_E_2001_10.pdf)
	 */

	/* check the the algorithmIDs from the AlgorithmInfo */
	for (i = 0; i < priv->algorithm_ids_in_tokeninfo_count; ++i) {
		unsigned int id = priv->algorithm_ids_in_tokeninfo[i];
		if (id == 0x86 || id == 0x88) {
			do_rsa_sig = 1;
		} else if (id == 0x8C || id == 0x8A) {
			do_rsa_pure_sig = 1;
		}
	}

	/* check if any operation was selected */
	if (do_rsa_sig == 0 && do_rsa_pure_sig == 0) {
		/* no operation selected. we just have to try both,
		 * for the lack of any better reasoning */
		sc_log(ctx, "I was unable to determine, whether this key can be used with RSA_SIG or RSA_PURE_SIG. I will just try both.");
		do_rsa_sig = 1;
		do_rsa_pure_sig = 1;
	}

	if(do_rsa_pure_sig == 1){
		sc_log(ctx, "trying RSA_PURE_SIG (padded DigestInfo)");
		r = do_compute_signature(card, data, datalen, out, outlen);
		if (r >= SC_SUCCESS)
			LOG_FUNC_RETURN(ctx, r);
	}

	if(do_rsa_sig == 1){
		u8 *buf = malloc(datalen);
		u8 *stripped_data = buf;
		size_t stripped_datalen = datalen;
		if (!buf)
			LOG_FUNC_RETURN(ctx, SC_ERROR_OUT_OF_MEMORY);
		memcpy(buf, data, datalen);
		data = buf;

		sc_log(ctx, "trying RSA_SIG (just the DigestInfo)");

		/* remove padding: first try pkcs1 bt01 padding */
		r = sc_pkcs1_strip_01_padding(ctx, data, datalen, stripped_data, &stripped_datalen);
		if (r != SC_SUCCESS) {
			/* no pkcs1 bt01 padding => let's try zero padding
			 * This can only work if the data tbs doesn't have a
			 * leading 0 byte.  */
			while (*stripped_data == 0 && stripped_datalen != 0) {
				++stripped_data;
				--stripped_datalen;
			}
		}
		sc_log(ctx, "trying to sign raw hash value with prefix");
		r = do_compute_signature(card, stripped_data, stripped_datalen, out, outlen);
		if (r >= SC_SUCCESS) {
			free(buf);
			LOG_FUNC_RETURN(ctx, r);
		}
		sc_log(ctx, "trying to sign stripped raw hash value (card is responsible for prefix)");
		r = sc_pkcs1_strip_digest_info_prefix(NULL, stripped_data, stripped_datalen, stripped_data, &stripped_datalen);
		if (r != SC_SUCCESS) {
			free(buf);
			LOG_FUNC_RETURN(ctx, r);
		}
		r = do_compute_signature(card, stripped_data, stripped_datalen, out, outlen);
		free(buf);
		LOG_FUNC_RETURN(ctx, r);
	}

	LOG_FUNC_RETURN(ctx, SC_ERROR_INTERNAL);
}