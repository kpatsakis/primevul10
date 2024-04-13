int EVP_MD_CTX_copy(EVP_MD_CTX *out, const EVP_MD_CTX *in)
	{
	EVP_MD_CTX_init(out);
	return EVP_MD_CTX_copy_ex(out, in);
	}