void EVP_MD_CTX_destroy(EVP_MD_CTX *ctx)
	{
	if (ctx)
		{
		EVP_MD_CTX_cleanup(ctx);
		OPENSSL_free(ctx);
		}
	}