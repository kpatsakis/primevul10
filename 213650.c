static void av1dmx_check_pid(GF_Filter *filter, GF_AV1DmxCtx *ctx)
{
	u8 *dsi;
	u32 dsi_size, crc;

	//no config or no config change
	if (ctx->is_av1 && !gf_list_count(ctx->state.frame_state.header_obus)) return;

	if (!ctx->opid) {
		ctx->opid = gf_filter_pid_new(filter);
		av1dmx_check_dur(filter, ctx);
	}
	dsi = NULL;
	dsi_size = 0;

	if (ctx->vp_cfg) {
		gf_odf_vp_cfg_write(ctx->vp_cfg, &dsi, &dsi_size, ctx->vp_cfg->codec_initdata_size ? GF_TRUE : GF_FALSE);
	} else if (ctx->is_av1) {
		//first or config changed, compute dsi
		while (gf_list_count(ctx->state.config->obu_array)) {
			GF_AV1_OBUArrayEntry *a = (GF_AV1_OBUArrayEntry*) gf_list_pop_back(ctx->state.config->obu_array);
			if (a->obu) gf_free(a->obu);
			gf_free(a);
		}
		dsi = NULL;
		dsi_size = 0;
		while (gf_list_count(ctx->state.frame_state.header_obus)) {
			GF_AV1_OBUArrayEntry *a = (GF_AV1_OBUArrayEntry*) gf_list_get(ctx->state.frame_state.header_obus, 0);
			gf_list_add(ctx->state.config->obu_array, a);
			gf_list_rem(ctx->state.frame_state.header_obus, 0);
		}
		gf_odf_av1_cfg_write(ctx->state.config, &dsi, &dsi_size);

		if ((!ctx->fps.num || !ctx->fps.den) && ctx->state.tb_num && ctx->state.tb_den && ! ( (ctx->state.tb_num<=1) && (ctx->state.tb_den<=1) ) ) {
			ctx->cur_fps.num = ctx->state.tb_num;
			ctx->cur_fps.den = ctx->state.tb_den;
		}

	}
	crc = gf_crc_32(dsi, dsi_size);

	if (crc == ctx->dsi_crc) {
		gf_free(dsi);
		return;
	}
	ctx->dsi_crc = crc;

	//copy properties at init or reconfig
	gf_filter_pid_copy_properties(ctx->opid, ctx->ipid);
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_STREAM_TYPE, & PROP_UINT(GF_STREAM_VISUAL));

	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_CODECID, & PROP_UINT(ctx->codecid));

	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_TIMESCALE, & PROP_UINT(ctx->cur_fps.num));
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_FPS, & PROP_FRAC(ctx->cur_fps));
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_WIDTH, & PROP_UINT(ctx->state.width));
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_HEIGHT, & PROP_UINT(ctx->state.height));

	if (ctx->duration.num)
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_DURATION, & PROP_FRAC64(ctx->duration));

	if (ctx->bitrate) {
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_BITRATE, & PROP_UINT(ctx->bitrate));
	}

	if (dsi && dsi_size)
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_DECODER_CONFIG, & PROP_DATA_NO_COPY(dsi, dsi_size));

	if (ctx->is_file && ctx->index) {
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_PLAYBACK_MODE, & PROP_UINT(GF_PLAYBACK_MODE_FASTFORWARD) );
	}

	if (ctx->is_av1) {
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_COLR_PRIMARIES, & PROP_UINT(ctx->state.color_primaries) );
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_COLR_TRANSFER, & PROP_UINT(ctx->state.transfer_characteristics) );
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_COLR_MX, & PROP_UINT(ctx->state.matrix_coefficients) );
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_COLR_RANGE, & PROP_BOOL(ctx->state.color_range) );
	}
}