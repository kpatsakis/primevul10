int EVP_DigestInit(EVP_MD_CTX *ctx, const EVP_MD *type)
	{
	EVP_MD_CTX_init(ctx);
	return EVP_DigestInit_ex(ctx, type, NULL);
	}