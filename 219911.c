u_compute_hash(char_u *hash)
{
    context_sha256_T	ctx;
    linenr_T		lnum;
    char_u		*p;

    sha256_start(&ctx);
    for (lnum = 1; lnum <= curbuf->b_ml.ml_line_count; ++lnum)
    {
	p = ml_get(lnum);
	sha256_update(&ctx, p, (UINT32_T)(STRLEN(p) + 1));
    }
    sha256_finish(&ctx, hash);
}