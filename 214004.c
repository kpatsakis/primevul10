static void nhmldump_finalize(GF_Filter *filter)
{
	GF_NHMLDumpCtx *ctx = gf_filter_get_udta(filter);
	if (ctx->bs_r) gf_bs_del(ctx->bs_r);
	if (ctx->bs_w) gf_bs_del(ctx->bs_w);
	if (ctx->nhml_buffer) gf_free(ctx->nhml_buffer);
	if (ctx->b64_buffer) gf_free(ctx->b64_buffer);
	if (ctx->info_file) gf_free(ctx->info_file);
	if (ctx->media_file) gf_free(ctx->media_file);
}