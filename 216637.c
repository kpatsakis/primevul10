cardos_set_security_env(sc_card_t *card,
			    const sc_security_env_t *env,
			    int se_num)
{
	cardos_data_t* priv = (cardos_data_t*)card->drv_data;
	sc_apdu_t apdu;
	u8	data[9];
	int	key_id, r;

	assert(card != NULL && env != NULL);

	if (!(env->flags & SC_SEC_ENV_KEY_REF_PRESENT) || env->key_ref_len != 1) {
		sc_log(card->ctx, "No or invalid key reference\n");
		return SC_ERROR_INVALID_ARGUMENTS;
	}
	priv->sec_env = env; /* pass on to crypto routines */

	/* key_ref includes card mechanism and key number
	 * But newer cards appear to get this some other way,
	 * We can use flags passed to know what OpenSC expects from the card
	 * and have derived what these machanisums are. 
	 * Newer cards may change how this is done
	 */

	key_id = env->key_ref[0];

	sc_format_apdu(card, &apdu, SC_APDU_CASE_3_SHORT, 0x22, 0, 0);
	if (card->type == SC_CARD_TYPE_CARDOS_CIE_V1) {
		cardos_restore_security_env(card, 0x30);
		apdu.p1 = 0xF1;
	} else {
		apdu.p1 = 0x41;
	}
	switch (env->operation) {
	case SC_SEC_OPERATION_DECIPHER:
		apdu.p2 = 0xB8;
		break;
	case SC_SEC_OPERATION_SIGN:
		apdu.p2 = 0xB6;
		break;
	default:
		return SC_ERROR_INVALID_ARGUMENTS;
	}

	if (card->type == SC_CARD_TYPE_CARDOS_V5_0 || card->type == SC_CARD_TYPE_CARDOS_V5_3) {
		/* some cards appear to have key_id be both Cryptographic mechanism reference 4 bits
		 * and key_ref 4 bits. But this limits card to 16 keys.
		 * TODO may need to be looked at at a later time
		 */
		/* Private key reference */
		data[0] = 0x84;
		data[1] = 0x01;
		data[2] = key_id & 0x0F;
		/* Usage qualifier byte */
		data[3] = 0x95;
		data[4] = 0x01;
		data[5] = 0x40;
		apdu.lc = apdu.datalen = 6;
		if (key_id & 0xF0) {
			/* Cryptographic mechanism reference */
			data[6] = 0x80;
			data[7] = 0x01;
			data[8] = key_id & 0xF0;
			apdu.lc = apdu.datalen = 9;
		} else if (priv->sec_env->algorithm_flags & SC_ALGORITHM_RSA_PAD_PKCS1) {
			/* TODO this may only apply to c903 cards */
			/* TODO or only for cards without any supported_algos or EIDComplient only */
			data[6] = 0x80;
			data[7] = 0x01;
			data[8] = 0x10;
			apdu.lc = apdu.datalen = 9;
		} else if (priv->sec_env->algorithm_flags & SC_ALGORITHM_ECDSA_RAW) {
			data[6] = 0x80;
			data[7] = 0x01;
			data[8] = 0x30;
			apdu.lc = apdu.datalen = 9;
		}
	} else {
		data[0] = 0x83;
		data[1] = 0x01;
		data[2] = key_id;
		apdu.lc = apdu.datalen = 3;
	}
	apdu.data = data;

	r = sc_transmit_apdu(card, &apdu);
	LOG_TEST_RET(card->ctx, r, "APDU transmit failed");

	r = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(card->ctx, r, "Card returned error");

	do   {
		const struct sc_supported_algo_info* algorithm_info = env->supported_algos;
		int i=0;
		int algorithm_id_count = 0;

		for(i=0;i<SC_MAX_SUPPORTED_ALGORITHMS;++i)  {
			struct sc_supported_algo_info alg = algorithm_info[i];

			if(alg.operations & SC_PKCS15_ALGO_OP_COMPUTE_SIGNATURE)  {
				unsigned int algorithm_id = alg.algo_ref;

				sc_log(card->ctx, "is signature");
				sc_log(card->ctx, "Adding ID %d at index %d", algorithm_id, algorithm_id_count);
				priv->algorithm_ids_in_tokeninfo[algorithm_id_count++] = algorithm_id;
			}
			sc_log(card->ctx, "reference=%d, mechanism=%d, operations=%d, algo_ref=%d",
					alg.reference, alg.mechanism, alg.operations, alg.algo_ref);
		}
		priv -> algorithm_ids_in_tokeninfo_count = algorithm_id_count;
	} while (0);

	LOG_FUNC_RETURN(card->ctx, r);
}