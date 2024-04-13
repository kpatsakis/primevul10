int EVP_MD_CTX_copy_ex(EVP_MD_CTX *out, const EVP_MD_CTX *in)
	{
	unsigned char *tmp_buf;
	if ((in == NULL) || (in->digest == NULL))
		{
		EVPerr(EVP_F_EVP_MD_CTX_COPY_EX,EVP_R_INPUT_NOT_INITIALIZED);
		return 0;
		}
#ifndef OPENSSL_NO_ENGINE
	/* Make sure it's safe to copy a digest context using an ENGINE */
	if (in->engine && !ENGINE_init(in->engine))
		{
		EVPerr(EVP_F_EVP_MD_CTX_COPY_EX,ERR_R_ENGINE_LIB);
		return 0;
		}
#endif

	if (out->digest == in->digest)
		{
		tmp_buf = out->md_data;
	    	EVP_MD_CTX_set_flags(out,EVP_MD_CTX_FLAG_REUSE);
		}
	else tmp_buf = NULL;
	EVP_MD_CTX_cleanup(out);
	memcpy(out,in,sizeof *out);

	if (in->md_data && out->digest->ctx_size)
		{
		if (tmp_buf)
			out->md_data = tmp_buf;
		else
			{
			out->md_data=OPENSSL_malloc(out->digest->ctx_size);
			if (!out->md_data)
				{
				EVPerr(EVP_F_EVP_MD_CTX_COPY_EX,ERR_R_MALLOC_FAILURE);
				return 0;
				}
			}
		memcpy(out->md_data,in->md_data,out->digest->ctx_size);
		}

	out->update = in->update;

	if (in->pctx)
		{
		out->pctx = EVP_PKEY_CTX_dup(in->pctx);
		if (!out->pctx)
			{
			EVP_MD_CTX_cleanup(out);
			return 0;
			}
		}

	if (out->digest->copy)
		return out->digest->copy(out,in);
	
	return 1;
	}