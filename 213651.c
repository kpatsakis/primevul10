GF_Err av1dmx_parse_vp9(GF_Filter *filter, GF_AV1DmxCtx *ctx)
{
	Bool key_frame = GF_FALSE;
	u64 frame_size = 0, pts = 0;
	u64 pos, pos_ivf_hdr;
	u32 width = 0, height = 0, renderWidth, renderHeight;
	u32 num_frames_in_superframe = 0, superframe_index_size = 0, i = 0;
	u32 frame_sizes[VP9_MAX_FRAMES_IN_SUPERFRAME];
	u8 *output;
	GF_Err e;

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
			GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("[IVF/VP9] Corrupted timestamp "LLU" less than previous timestamp "LLU", assuming concatenation\n", pts, ctx->last_pts));
			ctx->cumulated_dur = ctx->last_pts + ctx->cur_fps.den;
			ctx->cumulated_dur -= pts;
			pts = ctx->cumulated_dur;
		}
		ctx->last_pts = pts;
	}

	/*check if it is a superframe*/
	e = gf_media_vp9_parse_superframe(ctx->bs, frame_size, &num_frames_in_superframe, frame_sizes, &superframe_index_size);
	if (e) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[VP9Dmx] Error parsing superframe structure\n"));
		return e;
	}

	for (i = 0; i < num_frames_in_superframe; ++i) {
		u64 pos2 = gf_bs_get_position(ctx->bs);
		if (gf_media_vp9_parse_sample(ctx->bs, ctx->vp_cfg, &key_frame, &width, &height, &renderWidth, &renderHeight) != GF_OK) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[VP9Dmx] Error parsing frame\n"));
			return e;
		}
		e = gf_bs_seek(ctx->bs, pos2 + frame_sizes[i]);
		if (e) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[VP9Dmx] Seek bad param (offset "LLU") (1)", pos2 + frame_sizes[i]));
			return e;
		}
	}
	if (gf_bs_get_position(ctx->bs) + superframe_index_size != pos + frame_size) {
		GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("[VP9Dmx] Inconsistent IVF frame size of "LLU" bytes.\n", frame_size));
		GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("      Detected %d frames (+ %d bytes for the superframe index):\n", num_frames_in_superframe, superframe_index_size));
		for (i = 0; i < num_frames_in_superframe; ++i) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("         superframe %d, size is %u bytes\n", i, frame_sizes[i]));
		}
		GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("\n"));
	}
	e = gf_bs_seek(ctx->bs, pos + frame_size);
	if (e) {
		GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("[VP9Dmx] Seek bad param (offset "LLU") (2)", pos + frame_size));
		return e;
	}

	u32 pck_size = (u32)(gf_bs_get_position(ctx->bs) - pos);
	assert(pck_size == frame_size);

	//check pid state
	av1dmx_check_pid(filter, ctx);

	if (!ctx->opid) {
		return GF_OK;
	}

	if (!ctx->is_playing) {
		gf_bs_seek(ctx->bs, pos_ivf_hdr);
		return GF_EOS;
	}

	GF_FilterPacket *pck = gf_filter_pck_new_alloc(ctx->opid, pck_size, &output);
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


	if (key_frame) {
		gf_filter_pck_set_sap(pck, GF_FILTER_SAP_1);
	}

	if (ctx->deps) {
		u8 flags = 0;
		//dependsOn
		flags = (key_frame) ? 2 : 1;
		flags <<= 2;
		//dependedOn
		//flags |= 2;
		flags <<= 2;
		//hasRedundant
		//flags |= ctx->has_redundant ? 1 : 2;
		gf_filter_pck_set_dependency_flags(pck, flags);
	}

	gf_bs_seek(ctx->bs, pos);
	gf_bs_read_data(ctx->bs, output, pck_size);
	gf_filter_pck_send(pck);

	av1dmx_update_cts(ctx);
	return GF_OK;
}