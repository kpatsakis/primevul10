void EVP_MD_CTX_init(EVP_MD_CTX *ctx)
	{
	memset(ctx,'\0',sizeof *ctx);
	}