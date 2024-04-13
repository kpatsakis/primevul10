GF_Err av1dmx_process_buffer(GF_Filter *filter, GF_AV1DmxCtx *ctx, const char *data, u32 data_size, Bool is_copy)
{
	u32 last_obu_end = 0;
	GF_Err e = GF_OK;

	if (!ctx->bs) ctx->bs = gf_bs_new(data, data_size, GF_BITSTREAM_READ);
	else gf_bs_reassign_buffer(ctx->bs, data, data_size);

#ifndef GPAC_DISABLE_LOG
	if (ctx->bsdbg && gf_log_tool_level_on(GF_LOG_PARSER, GF_LOG_DEBUG))
		gf_bs_set_logger(ctx->bs, av1dmx_bs_log, ctx);
#endif

	//check ivf vs obu vs annexB
	e = av1dmx_check_format(filter, ctx, ctx->bs, &last_obu_end);
	if (e) return e;

	while (gf_bs_available(ctx->bs)) {

		if (ctx->is_vp9) {
			e = av1dmx_parse_vp9(filter, ctx);
		} else if (ctx->is_av1) {
			e = av1dmx_parse_av1(filter, ctx);
		} else {
			e = av1dmx_parse_ivf(filter, ctx);
		}

		if (e!=GF_EOS)
			last_obu_end = (u32) gf_bs_get_position(ctx->bs);

		if (e) {
			break;
		}
		if (!ctx->is_playing && ctx->opid)
			break;
	}

	if (is_copy && last_obu_end) {
		assert(ctx->buf_size>=last_obu_end);
		memmove(ctx->buffer, ctx->buffer+last_obu_end, sizeof(char) * (ctx->buf_size-last_obu_end));
		ctx->buf_size -= last_obu_end;
	}
	if (e==GF_EOS) return GF_OK;
	if (e==GF_BUFFER_TOO_SMALL) return GF_OK;
	return e;
}