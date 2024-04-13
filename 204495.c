int EVP_DigestInit_ex(EVP_MD_CTX *ctx, const EVP_MD *type, ENGINE *impl)
	{
	EVP_MD_CTX_clear_flags(ctx,EVP_MD_CTX_FLAG_CLEANED);
#ifndef OPENSSL_NO_ENGINE
	/* Whether it's nice or not, "Inits" can be used on "Final"'d contexts
	 * so this context may already have an ENGINE! Try to avoid releasing
	 * the previous handle, re-querying for an ENGINE, and having a
	 * reinitialisation, when it may all be unecessary. */
	if (ctx->engine && ctx->digest && (!type ||
			(type && (type->type == ctx->digest->type))))
		goto skip_to_init;
	if (type)
		{
		/* Ensure an ENGINE left lying around from last time is cleared
		 * (the previous check attempted to avoid this if the same
		 * ENGINE and EVP_MD could be used). */
		if(ctx->engine)
			ENGINE_finish(ctx->engine);
		if(impl)
			{
			if (!ENGINE_init(impl))
				{
				EVPerr(EVP_F_EVP_DIGESTINIT_EX,EVP_R_INITIALIZATION_ERROR);
				return 0;
				}
			}
		else
			/* Ask if an ENGINE is reserved for this job */
			impl = ENGINE_get_digest_engine(type->type);
		if(impl)
			{
			/* There's an ENGINE for this job ... (apparently) */
			const EVP_MD *d = ENGINE_get_digest(impl, type->type);
			if(!d)
				{
				/* Same comment from evp_enc.c */
				EVPerr(EVP_F_EVP_DIGESTINIT_EX,EVP_R_INITIALIZATION_ERROR);
				ENGINE_finish(impl);
				return 0;
				}
			/* We'll use the ENGINE's private digest definition */
			type = d;
			/* Store the ENGINE functional reference so we know
			 * 'type' came from an ENGINE and we need to release
			 * it when done. */
			ctx->engine = impl;
			}
		else
			ctx->engine = NULL;
		}
	else
	if(!ctx->digest)
		{
		EVPerr(EVP_F_EVP_DIGESTINIT_EX,EVP_R_NO_DIGEST_SET);
		return 0;
		}
#endif
	if (ctx->digest != type)
		{
		if (ctx->digest && ctx->digest->ctx_size)
			OPENSSL_free(ctx->md_data);
		ctx->digest=type;
		if (!(ctx->flags & EVP_MD_CTX_FLAG_NO_INIT) && type->ctx_size)
			{
			ctx->update = type->update;
			ctx->md_data=OPENSSL_malloc(type->ctx_size);
			if (ctx->md_data == NULL)
				{
				EVPerr(EVP_F_EVP_DIGESTINIT_EX,
							ERR_R_MALLOC_FAILURE);
				return 0;
				}
			}
		}
#ifndef OPENSSL_NO_ENGINE
skip_to_init:
#endif
	if (ctx->pctx)
		{
		int r;
		r = EVP_PKEY_CTX_ctrl(ctx->pctx, -1, EVP_PKEY_OP_TYPE_SIG,
					EVP_PKEY_CTRL_DIGESTINIT, 0, ctx);
		if (r <= 0 && (r != -2))
			return 0;
		}
	if (ctx->flags & EVP_MD_CTX_FLAG_NO_INIT)
		return 1;
#ifdef OPENSSL_FIPS
	if (FIPS_mode())
		{
		if (FIPS_digestinit(ctx, type))
			return 1;
		OPENSSL_free(ctx->md_data);
		ctx->md_data = NULL;
		return 0;
		}
#endif
	return ctx->digest->init(ctx);
	}