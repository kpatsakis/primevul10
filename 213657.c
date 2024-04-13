GF_Err av1dmx_check_format(GF_Filter *filter, GF_AV1DmxCtx *ctx, GF_BitStream *bs, u32 *last_obu_end)
{
	GF_Err e;
	if (last_obu_end) (*last_obu_end) = 0;
	//probing av1 bs mode
	if (ctx->bsmode != NOT_SET) return GF_OK;


	if (!ctx->state.config)
		ctx->state.config = gf_odf_av1_cfg_new();

	ctx->is_av1 = ctx->is_vp9 = ctx->is_vpX = GF_FALSE;
	ctx->codecid = 0;
	if (ctx->vp_cfg) gf_odf_vp_cfg_del(ctx->vp_cfg);
	ctx->vp_cfg = NULL;
	ctx->cur_fps = ctx->fps;
	if (!ctx->fps.num || !ctx->fps.den) {
		ctx->cur_fps.num = 25000;
		ctx->cur_fps.den = 1000;
	}

	ctx->pts_from_file = GF_FALSE;
	if (gf_media_probe_ivf(bs)) {
		u32 width = 0, height = 0;
		u32 codec_fourcc = 0, timebase_den = 0, timebase_num = 0, num_frames = 0;
		ctx->bsmode = IVF;

		e = gf_media_parse_ivf_file_header(bs, &width, &height, &codec_fourcc, &timebase_num, &timebase_den, &num_frames);
		if (e) return e;

		switch (codec_fourcc) {
		case GF_4CC('A', 'V', '0', '1'):
			ctx->is_av1 = GF_TRUE;
			ctx->codecid = GF_CODECID_AV1;
			break;
		case GF_4CC('V', 'P', '9', '0'):
			ctx->is_vp9 = GF_TRUE;
			ctx->codecid = GF_CODECID_VP9;
			ctx->vp_cfg = gf_odf_vp_cfg_new();
			break;
		case GF_4CC('V', 'P', '8', '0'):
			ctx->codecid = GF_CODECID_VP8;
			ctx->vp_cfg = gf_odf_vp_cfg_new();
			break;
		case GF_4CC('V', 'P', '1', '0'):
			ctx->codecid = GF_CODECID_VP10;
			ctx->vp_cfg = gf_odf_vp_cfg_new();
			GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("[IVF] %s parsing not implemented, import might be uncomplete or broken\n", gf_4cc_to_str(codec_fourcc) ));
			break;
		default:
			ctx->codecid = codec_fourcc;
			GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("[IVF] Unsupported codec FourCC %s\n", gf_4cc_to_str(codec_fourcc) ));
			return GF_NON_COMPLIANT_BITSTREAM;
		}
		if (ctx->vp_cfg && !ctx->is_vp9) {
			ctx->is_vpX = GF_TRUE;
			ctx->vp_cfg->profile = 1;
			ctx->vp_cfg->level = 10;
			ctx->vp_cfg->bit_depth = 8;
			//leave the rest as 0
		}

		ctx->state.width = ctx->state.width < width ? width : ctx->state.width;
		ctx->state.height = ctx->state.height < height ? height : ctx->state.height;
		ctx->state.tb_num = timebase_num;
		ctx->state.tb_den = timebase_den;

		if ((!ctx->fps.num || !ctx->fps.den) && ctx->state.tb_num && ctx->state.tb_den && ! ( (ctx->state.tb_num<=1) && (ctx->state.tb_den<=1) ) ) {
			ctx->cur_fps.num = ctx->state.tb_num;
			ctx->cur_fps.den = ctx->state.tb_den;
			GF_LOG(GF_LOG_DEBUG, GF_LOG_PARSER, ("[AV1Dmx] Detected IVF format FPS %d/%d\n", ctx->cur_fps.num, ctx->cur_fps.den));
			ctx->pts_from_file = GF_TRUE;
		} else {
			GF_LOG(GF_LOG_DEBUG, GF_LOG_PARSER, ("[AV1Dmx] Detected IVF format\n"));
		}
		ctx->file_hdr_size = (u32) gf_bs_get_position(bs);
		if (last_obu_end) (*last_obu_end) = (u32) gf_bs_get_position(bs);
		return GF_OK;
	} else if (gf_media_aom_probe_annexb(bs)) {
		GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[AV1Dmx] Detected Annex B format\n"));
		ctx->bsmode = AnnexB;
	} else {
		gf_bs_seek(bs, 0);
		e = aom_av1_parse_temporal_unit_from_section5(bs, &ctx->state);
		if (e && !gf_list_count(ctx->state.frame_state.frame_obus) ) {
			gf_filter_setup_failure(filter, e);
			ctx->bsmode = UNSUPPORTED;
			return e;
		}
		if (ctx->state.obu_type != OBU_TEMPORAL_DELIMITER) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[AV1Dmx] Error OBU stream start with %s, not a temporal delimiter - NOT SUPPORTED\n", gf_av1_get_obu_name(ctx->state.obu_type) ));
			gf_filter_setup_failure(filter, e);
			ctx->bsmode = UNSUPPORTED;
			return e;
		}
		GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[AV1Dmx] Detected OBUs Section 5 format\n"));
		ctx->bsmode = OBUs;

		gf_av1_reset_state(&ctx->state, GF_FALSE);
		gf_bs_seek(bs, 0);
	}
	ctx->is_av1 = GF_TRUE;
	ctx->state.unframed = GF_TRUE;
	ctx->codecid = GF_CODECID_AV1;
	return GF_OK;
}