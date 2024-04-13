static void av1dmx_check_dur(GF_Filter *filter, GF_AV1DmxCtx *ctx)
{
	FILE *stream;
	GF_Err e;
	GF_BitStream *bs;
	u64 duration, cur_dur, last_cdur, rate;
	AV1State av1state;
	const char *filepath=NULL;
	const GF_PropertyValue *p;
	if (!ctx->opid || ctx->timescale || ctx->file_loaded) return;

	p = gf_filter_pid_get_property(ctx->ipid, GF_PROP_PID_FILE_CACHED);
	if (p && p->value.boolean) ctx->file_loaded = GF_TRUE;

	p = gf_filter_pid_get_property(ctx->ipid, GF_PROP_PID_FILEPATH);
	if (!p || !p->value.string || !strncmp(p->value.string, "gmem://", 7)) {
		ctx->is_file = GF_FALSE;
		ctx->file_loaded = GF_TRUE;
		return;
	}
	filepath = p->value.string;
	ctx->is_file = GF_TRUE;

	if (ctx->index<0) {
		if (gf_opts_get_bool("temp", "force_indexing")) {
			ctx->index = 1.0;
		} else {
			p = gf_filter_pid_get_property(ctx->ipid, GF_PROP_PID_DOWN_SIZE);
			if (!p || (p->value.longuint > 100000000)) {
				GF_LOG(GF_LOG_INFO, GF_LOG_PARSER, ("[AV1/VP9] Source file larger than 100M, skipping indexing\n"));
			} else {
				ctx->index = -ctx->index;
			}
		}
	}
	if (ctx->index<=0)
		return;

	stream = gf_fopen(filepath, "rb");
	if (!stream) return;

	ctx->index_size = 0;

	bs = gf_bs_from_file(stream, GF_BITSTREAM_READ);

	if (ctx->file_hdr_size) {
		gf_bs_seek(bs, ctx->file_hdr_size);
	}
	gf_av1_init_state(&av1state);
	av1state.skip_frames = GF_TRUE;
	av1state.config = gf_odf_av1_cfg_new();

	duration = 0;
	cur_dur = last_cdur = 0;
	while (gf_bs_available(bs)) {
		Bool is_sap=GF_FALSE;
		u64 pts = GF_FILTER_NO_TS;
		u64 frame_start = gf_bs_get_position(bs);
		gf_av1_reset_state(&av1state, GF_FALSE);

		/*we process each TU and extract only the necessary OBUs*/
		switch (ctx->bsmode) {
		case OBUs:
			e = aom_av1_parse_temporal_unit_from_section5(bs, &av1state);
			break;
		case AnnexB:
			e = aom_av1_parse_temporal_unit_from_annexb(bs, &av1state);
			break;
		case IVF:
			if (ctx->is_av1) {
				e = aom_av1_parse_temporal_unit_from_ivf(bs, &av1state);
			} else {
				u64 frame_size;
				e = gf_media_parse_ivf_frame_header(bs, &frame_size, &pts);
				if (!e) gf_bs_skip_bytes(bs, frame_size);
		 		is_sap = GF_TRUE;
			}
			break;
		default:
			e = GF_NOT_SUPPORTED;
		}
		if (e)
		 	break;

		if (pts != GF_FILTER_NO_TS) {
			duration = pts;
			cur_dur = pts - last_cdur;
		} else {
			duration += ctx->cur_fps.den;
			cur_dur += ctx->cur_fps.den;
		}
		if (av1state.frame_state.key_frame)
		 	is_sap = GF_TRUE;

		//only index at I-frame start
		if (frame_start && is_sap && (cur_dur > ctx->index * ctx->cur_fps.num) ) {
			if (!ctx->index_alloc_size) ctx->index_alloc_size = 10;
			else if (ctx->index_alloc_size == ctx->index_size) ctx->index_alloc_size *= 2;
			ctx->indexes = gf_realloc(ctx->indexes, sizeof(AV1Idx)*ctx->index_alloc_size);
			ctx->indexes[ctx->index_size].pos = frame_start;
			ctx->indexes[ctx->index_size].duration = (Double) duration;
			ctx->indexes[ctx->index_size].duration /= ctx->cur_fps.num;
			ctx->index_size ++;
			last_cdur = cur_dur;
			cur_dur = 0;
		}
	}
	rate = gf_bs_get_position(bs);
	gf_bs_del(bs);
	gf_fclose(stream);
	gf_odf_av1_cfg_del(av1state.config);
	gf_av1_reset_state(&av1state, GF_TRUE);

	if (!ctx->duration.num || (ctx->duration.num  * ctx->cur_fps.num != duration * ctx->duration.den)) {
		ctx->duration.num = (s32) duration;
		ctx->duration.den = ctx->cur_fps.num;

		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_DURATION, & PROP_FRAC64(ctx->duration));

		if (duration && (!gf_sys_is_test_mode() || gf_opts_get_bool("temp", "force_indexing"))) {
			rate *= 8 * ctx->duration.den;
			rate /= ctx->duration.num;
			ctx->bitrate = (u32) rate;
		}
	}

	//currently not supported because of OBU size field rewrite - could work on some streams but we would
	//need to analyse all OBUs in the stream for that
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_CAN_DATAREF, & PROP_BOOL(GF_FALSE) );
}