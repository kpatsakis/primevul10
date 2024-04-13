iasecc_set_security_env(struct sc_card *card,
		const struct sc_security_env *env, int se_num)
{
	struct sc_context *ctx = card->ctx;
	struct iasecc_sdo sdo;
	struct iasecc_private_data *prv = (struct iasecc_private_data *) card->drv_data;
	unsigned algo_ref;
	struct sc_apdu apdu;
	unsigned sign_meth, sign_ref, auth_meth, auth_ref, aflags;
	unsigned char cse_crt_at[] = {
		0x84, 0x01, 0xFF,
		0x80, 0x01, IASECC_ALGORITHM_RSA_PKCS
	};
	unsigned char cse_crt_dst[] = {
		0x84, 0x01, 0xFF,
		0x80, 0x01, (IASECC_ALGORITHM_RSA_PKCS | IASECC_ALGORITHM_SHA1)
	};
	unsigned char cse_crt_ht[] = {
		0x80, 0x01, IASECC_ALGORITHM_SHA1
	};
	unsigned char cse_crt_ct[] = {
		0x84, 0x01, 0xFF,
		0x80, 0x01, (IASECC_ALGORITHM_RSA_PKCS_DECRYPT | IASECC_ALGORITHM_SHA1)
	};
	int rv, operation = env->operation;

	/* TODO: take algorithm references from 5032, not from header file. */
	LOG_FUNC_CALLED(ctx);
	sc_log(ctx, "iasecc_set_security_env(card:%p) operation 0x%X; senv.algorithm 0x%X, senv.algorithm_ref 0x%X",
			card, env->operation, env->algorithm, env->algorithm_ref);

	memset(&sdo, 0, sizeof(sdo));
	sdo.sdo_class = IASECC_SDO_CLASS_RSA_PRIVATE;
	sdo.sdo_ref  = env->key_ref[0] & ~IASECC_OBJECT_REF_LOCAL;
	rv = iasecc_sdo_get_data(card, &sdo);
	LOG_TEST_RET(ctx, rv, "Cannot get RSA PRIVATE SDO data");

	/* To made by iasecc_sdo_convert_to_file() */
	prv->key_size = *(sdo.docp.size.value + 0) * 0x100 + *(sdo.docp.size.value + 1);
	sc_log(ctx, "prv->key_size 0x%"SC_FORMAT_LEN_SIZE_T"X", prv->key_size);

	rv = iasecc_sdo_convert_acl(card, &sdo, SC_AC_OP_PSO_COMPUTE_SIGNATURE, &sign_meth, &sign_ref);
	LOG_TEST_RET(ctx, rv, "Cannot convert SC_AC_OP_SIGN acl");

	rv = iasecc_sdo_convert_acl(card, &sdo, SC_AC_OP_INTERNAL_AUTHENTICATE, &auth_meth, &auth_ref);
	LOG_TEST_RET(ctx, rv, "Cannot convert SC_AC_OP_INT_AUTH acl");

	aflags = env->algorithm_flags;

	if (!(aflags & SC_ALGORITHM_RSA_PAD_PKCS1))
		LOG_TEST_RET(ctx, SC_ERROR_NOT_SUPPORTED, "Only supported signature with PKCS1 padding");

	if (operation == SC_SEC_OPERATION_SIGN)   {
		if (!(aflags & (SC_ALGORITHM_RSA_HASH_SHA1 | SC_ALGORITHM_RSA_HASH_SHA256)))   {
			sc_log(ctx, "CKM_RSA_PKCS asked -- use 'AUTHENTICATE' sign operation instead of 'SIGN'");
			operation = SC_SEC_OPERATION_AUTHENTICATE;
		}
		else if (sign_meth == SC_AC_NEVER)   {
			LOG_TEST_RET(ctx, SC_ERROR_NOT_SUPPORTED, "PSO_DST not allowed for this key");
		}
	}

	if (operation == SC_SEC_OPERATION_SIGN)   {
		prv->op_method = sign_meth;
		prv->op_ref = sign_ref;
	}
	else if (operation == SC_SEC_OPERATION_AUTHENTICATE)   {
		if (auth_meth == SC_AC_NEVER)
			LOG_TEST_RET(ctx, SC_ERROR_NOT_ALLOWED, "INTERNAL_AUTHENTICATE is not allowed for this key");

		prv->op_method = auth_meth;
		prv->op_ref = auth_ref;
	}

	sc_log(ctx, "senv.algorithm 0x%X, senv.algorithm_ref 0x%X", env->algorithm, env->algorithm_ref);
	sc_log(ctx,
	       "se_num %i, operation 0x%X, algorithm 0x%X, algorithm_ref 0x%X, flags 0x%X; key size %"SC_FORMAT_LEN_SIZE_T"u",
	       se_num, operation, env->algorithm, env->algorithm_ref,
	       env->algorithm_flags, prv->key_size);
	switch (operation)  {
	case SC_SEC_OPERATION_SIGN:
		if (!(env->algorithm_flags & SC_ALGORITHM_RSA_PAD_PKCS1))
			LOG_TEST_RET(ctx, SC_ERROR_INVALID_ARGUMENTS, "Need RSA_PKCS1 specified");

		if (env->algorithm_flags & SC_ALGORITHM_RSA_HASH_SHA256)   {
			algo_ref = iasecc_get_algorithm(ctx, env, SC_PKCS15_ALGO_OP_HASH, CKM_SHA256);
			if (!algo_ref)
				LOG_TEST_RET(ctx, SC_ERROR_NOT_SUPPORTED, "Card application do not supports HASH:SHA256");

			cse_crt_ht[2] = algo_ref; /* IASECC_ALGORITHM_SHA2 */

			algo_ref = iasecc_get_algorithm(ctx, env, SC_PKCS15_ALGO_OP_COMPUTE_SIGNATURE,  CKM_SHA256_RSA_PKCS);
			if (!algo_ref)
				LOG_TEST_RET(ctx, SC_ERROR_NOT_SUPPORTED, "Card application do not supports SIGNATURE:SHA1_RSA_PKCS");

			cse_crt_dst[2] = env->key_ref[0] | IASECC_OBJECT_REF_LOCAL;
			cse_crt_dst[5] = algo_ref;   /* IASECC_ALGORITHM_RSA_PKCS | IASECC_ALGORITHM_SHA2 */
		}
		else if (env->algorithm_flags & SC_ALGORITHM_RSA_HASH_SHA1)   {
			algo_ref = iasecc_get_algorithm(ctx, env, SC_PKCS15_ALGO_OP_HASH,  CKM_SHA_1);
			if (!algo_ref)
				LOG_TEST_RET(ctx, SC_ERROR_NOT_SUPPORTED, "Card application do not supports HASH:SHA1");

			cse_crt_ht[2] = algo_ref;	/* IASECC_ALGORITHM_SHA1 */

			algo_ref = iasecc_get_algorithm(ctx, env, SC_PKCS15_ALGO_OP_COMPUTE_SIGNATURE,  CKM_SHA1_RSA_PKCS);
			if (!algo_ref)
				LOG_TEST_RET(ctx, SC_ERROR_NOT_SUPPORTED, "Card application do not supports SIGNATURE:SHA1_RSA_PKCS");

			cse_crt_dst[2] = env->key_ref[0] | IASECC_OBJECT_REF_LOCAL;
			cse_crt_dst[5] = algo_ref;   /* IASECC_ALGORITHM_RSA_PKCS | IASECC_ALGORITHM_SHA1 */
		}
		else   {
			LOG_TEST_RET(ctx, SC_ERROR_INVALID_ARGUMENTS, "Need RSA_HASH_SHA[1,256] specified");
		}

		sc_format_apdu(card, &apdu, SC_APDU_CASE_3_SHORT, 0x22, 0x41, IASECC_CRT_TAG_HT);
		apdu.data = cse_crt_ht;
		apdu.datalen = sizeof(cse_crt_ht);
		apdu.lc = sizeof(cse_crt_ht);

		rv = sc_transmit_apdu(card, &apdu);
		LOG_TEST_RET(ctx, rv, "APDU transmit failed");
		rv = sc_check_sw(card, apdu.sw1, apdu.sw2);
		LOG_TEST_RET(ctx, rv, "MSE restore error");

		sc_format_apdu(card, &apdu, SC_APDU_CASE_3_SHORT, 0x22, 0x41, IASECC_CRT_TAG_DST);
		apdu.data = cse_crt_dst;
		apdu.datalen = sizeof(cse_crt_dst);
		apdu.lc = sizeof(cse_crt_dst);
		break;
	case SC_SEC_OPERATION_AUTHENTICATE:
		algo_ref = iasecc_get_algorithm(ctx, env, SC_PKCS15_ALGO_OP_COMPUTE_SIGNATURE,  CKM_RSA_PKCS);
		if (!algo_ref)
			LOG_TEST_RET(ctx, SC_ERROR_NOT_SUPPORTED, "Application do not supports SIGNATURE:RSA_PKCS");

		cse_crt_at[2] = env->key_ref[0] | IASECC_OBJECT_REF_LOCAL;
		cse_crt_at[5] = algo_ref;	/* IASECC_ALGORITHM_RSA_PKCS */

		sc_format_apdu(card, &apdu, SC_APDU_CASE_3_SHORT, 0x22, 0x41, IASECC_CRT_TAG_AT);
		apdu.data = cse_crt_at;
		apdu.datalen = sizeof(cse_crt_at);
		apdu.lc = sizeof(cse_crt_at);
		break;
	case SC_SEC_OPERATION_DECIPHER:
		rv = iasecc_sdo_convert_acl(card, &sdo, SC_AC_OP_PSO_DECRYPT, &prv->op_method, &prv->op_ref);
		LOG_TEST_RET(ctx, rv, "Cannot convert SC_AC_OP_PSO_DECRYPT acl");
		algo_ref = iasecc_get_algorithm(ctx, env, SC_PKCS15_ALGO_OP_DECIPHER,  CKM_RSA_PKCS);
		if (!algo_ref)
			LOG_TEST_RET(ctx, SC_ERROR_NOT_SUPPORTED, "Application do not supports DECIPHER:RSA_PKCS");

		cse_crt_ct[2] = env->key_ref[0] | IASECC_OBJECT_REF_LOCAL;
		cse_crt_ct[5] = algo_ref;	/* IASECC_ALGORITHM_RSA_PKCS_DECRYPT | IASECC_ALGORITHM_SHA1 */

		sc_format_apdu(card, &apdu, SC_APDU_CASE_3_SHORT, 0x22, 0x41, IASECC_CRT_TAG_CT);
		apdu.data = cse_crt_ct;
		apdu.datalen = sizeof(cse_crt_ct);
		apdu.lc = sizeof(cse_crt_ct);
		break;
	default:
		LOG_FUNC_RETURN(ctx, SC_ERROR_NOT_SUPPORTED);
	}

	rv = sc_transmit_apdu(card, &apdu);
	LOG_TEST_RET(ctx, rv, "APDU transmit failed");
	rv = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(ctx, rv, "MSE restore error");

	prv->security_env = *env;
	prv->security_env.operation = operation;

	LOG_FUNC_RETURN(ctx, 0);
}