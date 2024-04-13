static void av1dmx_finalize(GF_Filter *filter)
{
	GF_AV1DmxCtx *ctx = gf_filter_get_udta(filter);
	if (ctx->bs) gf_bs_del(ctx->bs);
	if (ctx->indexes) gf_free(ctx->indexes);

	gf_av1_reset_state(&ctx->state, GF_TRUE);
	if (ctx->state.config) gf_odf_av1_cfg_del(ctx->state.config);
	if (ctx->state.bs) gf_bs_del(ctx->state.bs);
	if (ctx->state.frame_obus) gf_free(ctx->state.frame_obus);
	if (ctx->buffer) gf_free(ctx->buffer);

	if (ctx->vp_cfg) gf_odf_vp_cfg_del(ctx->vp_cfg);
}