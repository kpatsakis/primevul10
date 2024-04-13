iasecc_compute_signature(struct sc_card *card,
		const unsigned char *in, size_t in_len, unsigned char *out, size_t out_len)
{
	struct sc_context *ctx;
	struct iasecc_private_data *prv;
	struct sc_security_env *env;

	if (!card || !in || !out)
		return SC_ERROR_INVALID_ARGUMENTS;

	ctx = card->ctx;
	prv = (struct iasecc_private_data *) card->drv_data;
	env = &prv->security_env;

	LOG_FUNC_CALLED(ctx);
	sc_log(ctx,
	       "inlen %"SC_FORMAT_LEN_SIZE_T"u, outlen %"SC_FORMAT_LEN_SIZE_T"u",
	       in_len, out_len);

	if (env->operation == SC_SEC_OPERATION_SIGN)
		return iasecc_compute_signature_dst(card, in, in_len, out,  out_len);
	else if (env->operation == SC_SEC_OPERATION_AUTHENTICATE)
		return iasecc_compute_signature_at(card, in, in_len, out,  out_len);

	LOG_FUNC_RETURN(ctx, SC_ERROR_NOT_SUPPORTED);
}