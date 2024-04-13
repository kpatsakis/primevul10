int EVP_Digest(const void *data, size_t count,
		unsigned char *md, unsigned int *size, const EVP_MD *type, ENGINE *impl)
	{
	EVP_MD_CTX ctx;
	int ret;

	EVP_MD_CTX_init(&ctx);
	EVP_MD_CTX_set_flags(&ctx,EVP_MD_CTX_FLAG_ONESHOT);
	ret=EVP_DigestInit_ex(&ctx, type, impl)
	  && EVP_DigestUpdate(&ctx, data, count)
	  && EVP_DigestFinal_ex(&ctx, md, size);
	EVP_MD_CTX_cleanup(&ctx);

	return ret;
	}