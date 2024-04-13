EVP_MD_CTX *EVP_MD_CTX_create(void)
	{
	EVP_MD_CTX *ctx=OPENSSL_malloc(sizeof *ctx);

	if (ctx)
		EVP_MD_CTX_init(ctx);

	return ctx;
	}