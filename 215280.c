iasecc_qsign_data_sha256(struct sc_context *ctx, const unsigned char *in, size_t in_len,
				struct iasecc_qsign_data *out)
{
	SHA256_CTX sha256;
	SHA_LONG pre_hash_Nl;
	int jj, ii;
	int hh_size = sizeof(SHA_LONG), hh_num = SHA256_DIGEST_LENGTH / sizeof(SHA_LONG);

	LOG_FUNC_CALLED(ctx);
	if (!in || !in_len || !out)
		LOG_FUNC_RETURN(ctx, SC_ERROR_INVALID_ARGUMENTS);

	sc_log(ctx,
	       "sc_pkcs15_get_qsign_data() input data length %"SC_FORMAT_LEN_SIZE_T"u",
	       in_len);
	memset(out, 0, sizeof(struct iasecc_qsign_data));

	SHA256_Init(&sha256);
	SHA256_Update(&sha256, in, in_len);

	for (jj=0; jj<hh_num; jj++)
		for(ii=0; ii<hh_size; ii++)
			out->pre_hash[jj*hh_size + ii] = ((sha256.h[jj] >> 8*(hh_size-1-ii)) & 0xFF);
	out->pre_hash_size = SHA256_DIGEST_LENGTH;
	sc_log(ctx, "Pre hash:%s", sc_dump_hex(out->pre_hash, out->pre_hash_size));

	pre_hash_Nl = sha256.Nl - (sha256.Nl % (sizeof(sha256.data) * 8));
	for (ii=0; ii<hh_size; ii++)   {
		out->counter[ii] = (sha256.Nh >> 8*(hh_size-1-ii)) &0xFF;
		out->counter[hh_size+ii] = (pre_hash_Nl >> 8*(hh_size-1-ii)) &0xFF;
	}
	for (ii=0, out->counter_long=0; ii<(int)sizeof(out->counter); ii++)
		out->counter_long = out->counter_long*0x100 + out->counter[ii];
	sc_log(ctx, "Pre counter(%li):%s", out->counter_long, sc_dump_hex(out->counter, sizeof(out->counter)));

	if (sha256.num)   {
		memcpy(out->last_block, in + in_len - sha256.num, sha256.num);
		out->last_block_size = sha256.num;
		sc_log(ctx, "Last block(%"SC_FORMAT_LEN_SIZE_T"u):%s",
		       out->last_block_size,
		       sc_dump_hex(out->last_block, out->last_block_size));
	}

	SHA256_Final(out->hash, &sha256);
	out->hash_size = SHA256_DIGEST_LENGTH;
	sc_log(ctx, "Expected digest %s\n", sc_dump_hex(out->hash, out->hash_size));

	LOG_FUNC_RETURN(ctx, SC_SUCCESS);
}