static GFINLINE void av1dmx_update_cts(GF_AV1DmxCtx *ctx)
{
	assert(ctx->cur_fps.num);
	assert(ctx->cur_fps.den);

	if (ctx->timescale) {
		u64 inc = ctx->cur_fps.den;
		inc *= ctx->timescale;
		inc /= ctx->cur_fps.num;
		ctx->cts += inc;
	} else {
		ctx->cts += ctx->cur_fps.den;
	}
}