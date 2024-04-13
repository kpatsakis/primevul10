GF_Err av1dmx_parse_ivf(GF_Filter *filter, GF_AV1DmxCtx *ctx)
{
	GF_Err e;
	u32 pck_size;
	u64 frame_size = 0, pts = GF_FILTER_NO_TS;
	GF_FilterPacket *pck;
	u64 pos, pos_ivf_hdr;
	u8 *output;

	pos_ivf_hdr = gf_bs_get_position(ctx->bs);
	e = gf_media_parse_ivf_frame_header(ctx->bs, &frame_size, &pts);
	if (e) return e;

	pos = gf_bs_get_position(ctx->bs);
	if (gf_bs_available(ctx->bs) < frame_size) {
		gf_bs_seek(ctx->bs, pos_ivf_hdr);
		return GF_EOS;
	}

	if (ctx->pts_from_file) {
		pts += ctx->cumulated_dur;
		if (ctx->last_pts && (ctx->last_pts>pts)) {
			pts -= ctx->cumulated_dur;
			GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("[IVF/AV1] Corrupted timestamp "LLU" less than previous timestamp "LLU", assuming concatenation\n", pts, ctx->last_pts));
			ctx->cumulated_dur = ctx->last_pts + ctx->cur_fps.den;
			ctx->cumulated_dur -= pts;
			pts = ctx->cumulated_dur;
		}
		ctx->last_pts = pts;
	}


	//check pid state
	av1dmx_check_pid(filter, ctx);

	if (!ctx->opid) {
		return GF_OK;
	}

	if (!ctx->is_playing) {
		gf_bs_seek(ctx->bs, pos_ivf_hdr);
		return GF_EOS;
	}

	pck_size = (u32)frame_size;
	pck = gf_filter_pck_new_alloc(ctx->opid, pck_size, &output);
	if (!pck) {
		gf_bs_seek(ctx->bs, pos_ivf_hdr);
		return GF_OUT_OF_MEM;
	}
	if (ctx->src_pck) gf_filter_pck_merge_properties(ctx->src_pck, pck);

	if (ctx->pts_from_file) {
		gf_filter_pck_set_cts(pck, pts);
	} else {
		gf_filter_pck_set_cts(pck, ctx->cts);
	}

	gf_bs_seek(ctx->bs, pos);
	gf_bs_read_data(ctx->bs, output, pck_size);

	if (output[0] & 0x80)
		gf_filter_pck_set_sap(pck, GF_FILTER_SAP_1);
	else
		gf_filter_pck_set_sap(pck, GF_FILTER_SAP_NONE);

	gf_filter_pck_send(pck);

	av1dmx_update_cts(ctx);
	return GF_OK;
}