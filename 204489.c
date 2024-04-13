int EVP_DigestUpdate(EVP_MD_CTX *ctx, const void *data, size_t count)
	{
#ifdef OPENSSL_FIPS
	return FIPS_digestupdate(ctx, data, count);
#else
	return ctx->update(ctx,data,count);
#endif
	}