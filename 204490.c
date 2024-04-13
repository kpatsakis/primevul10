int EVP_DigestFinal(EVP_MD_CTX *ctx, unsigned char *md, unsigned int *size)
	{
	int ret;
	ret = EVP_DigestFinal_ex(ctx, md, size);
	EVP_MD_CTX_cleanup(ctx);
	return ret;
	}