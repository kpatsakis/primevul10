int EVP_DigestFinal_ex(EVP_MD_CTX *ctx, unsigned char *md, unsigned int *size)
	{
#ifdef OPENSSL_FIPS
	return FIPS_digestfinal(ctx, md, size);
#else
	int ret;

	OPENSSL_assert(ctx->digest->md_size <= EVP_MAX_MD_SIZE);
	ret=ctx->digest->final(ctx,md);
	if (size != NULL)
		*size=ctx->digest->md_size;
	if (ctx->digest->cleanup)
		{
		ctx->digest->cleanup(ctx);
		EVP_MD_CTX_set_flags(ctx,EVP_MD_CTX_FLAG_CLEANED);
		}
	memset(ctx->md_data,0,ctx->digest->ctx_size);
	return ret;
#endif
	}