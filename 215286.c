iasecc_qsign_data_sha1(struct sc_context *ctx, const unsigned char *in, size_t in_len,
				struct iasecc_qsign_data *out)
{
	SHA_CTX sha;
	SHA_LONG pre_hash_Nl, *hh[5] = {
		&sha.h0, &sha.h1, &sha.h2, &sha.h3, &sha.h4
	};
	int jj, ii;
	int hh_size = sizeof(SHA_LONG), hh_num = SHA_DIGEST_LENGTH / sizeof(SHA_LONG);

	LOG_FUNC_CALLED(ctx);

	if (!in || !in_len || !out)
		LOG_FUNC_RETURN(ctx, SC_ERROR_INVALID_ARGUMENTS);

	sc_log(ctx,
	       "sc_pkcs15_get_qsign_data() input data length %"SC_FORMAT_LEN_SIZE_T"u",
	       in_len);
	memset(out, 0, sizeof(struct iasecc_qsign_data));

	SHA1_Init(&sha);
	SHA1_Update(&sha, in, in_len);

	for (jj=0; jj<hh_num; jj++)
		for(ii=0; ii<hh_size; ii++)
			out->pre_hash[jj*hh_size + ii] = ((*hh[jj] >> 8*(hh_size-1-ii)) & 0xFF);
	out->pre_hash_size = SHA_DIGEST_LENGTH;
	sc_log(ctx, "Pre SHA1:%s", sc_dump_hex(out->pre_hash, out->pre_hash_size));

	pre_hash_Nl = sha.Nl - (sha.Nl % (sizeof(sha.data) * 8));
	for (ii=0; ii<hh_size; ii++)   {
		out->counter[ii] = (sha.Nh >> 8*(hh_size-1-ii)) &0xFF;
		out->counter[hh_size+ii] = (pre_hash_Nl >> 8*(hh_size-1-ii)) &0xFF;
	}
	for (ii=0, out->counter_long=0; ii<(int)sizeof(out->counter); ii++)
		out->counter_long = out->counter_long*0x100 + out->counter[ii];
	sc_log(ctx, "Pre counter(%li):%s", out->counter_long, sc_dump_hex(out->counter, sizeof(out->counter)));

	if (sha.num)   {
		memcpy(out->last_block, in + in_len - sha.num, sha.num);
		out->last_block_size = sha.num;
		sc_log(ctx, "Last block(%"SC_FORMAT_LEN_SIZE_T"u):%s",
		       out->last_block_size,
		       sc_dump_hex(out->last_block, out->last_block_size));
	}

	SHA1_Final(out->hash, &sha);
	out->hash_size = SHA_DIGEST_LENGTH;
	sc_log(ctx, "Expected digest %s\n", sc_dump_hex(out->hash, out->hash_size));

	LOG_FUNC_RETURN(ctx, SC_SUCCESS);
}