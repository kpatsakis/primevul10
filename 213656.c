GF_Err av1dmx_parse_av1(GF_Filter *filter, GF_AV1DmxCtx *ctx)
{
	GF_Err e = GF_OK;
	u64 start;

	if (!ctx->is_playing) {
		ctx->state.frame_state.is_first_frame = GF_TRUE;
	}

	/*we process each TU and extract only the necessary OBUs*/
	start = gf_bs_get_position(ctx->bs);
	switch (ctx->bsmode) {
	case OBUs:
		//first frame loaded !
		if (ctx->state.bs && gf_bs_get_position(ctx->state.bs) && (ctx->state.obu_type == OBU_TEMPORAL_DELIMITER)) {
			e = GF_OK;
		} else {
			e = aom_av1_parse_temporal_unit_from_section5(ctx->bs, &ctx->state);
		}
		break;
	case AnnexB:
		//first TU loaded !
		if (ctx->state.bs && gf_bs_get_position(ctx->state.bs)) {
			e = GF_OK;
		} else {
			e = aom_av1_parse_temporal_unit_from_annexb(ctx->bs, &ctx->state);
			if (e==GF_BUFFER_TOO_SMALL) {
				gf_av1_reset_state(&ctx->state, GF_FALSE);
				gf_bs_seek(ctx->bs, start);
			}
		}
		break;
	case IVF:
		//first frame loaded !
		if (ctx->state.bs && gf_bs_get_position(ctx->state.bs)) {
			e = GF_OK;
		} else {
			e = aom_av1_parse_temporal_unit_from_ivf(ctx->bs, &ctx->state);
		}
		break;
	default:
		e = GF_NOT_SUPPORTED;
	}

	//check pid state
	av1dmx_check_pid(filter, ctx);

	if (e) return e;


	if (!ctx->opid) {
		if (ctx->state.obu_type != OBU_TEMPORAL_DELIMITER) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[AV1Dmx] output pid not configured (no sequence header yet ?), skipping OBU\n"));
		}
		gf_av1_reset_state(&ctx->state, GF_FALSE);
		return GF_OK;
	}

	if (!ctx->is_playing) {
		//don't reset state we would skip seq header obu in first frame
		//gf_av1_reset_state(&ctx->state, GF_FALSE);
		return GF_OK;
	}

	return av1dmx_parse_flush_sample(filter, ctx);

}