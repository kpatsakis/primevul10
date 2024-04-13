static const char * av1dmx_probe_data(const u8 *data, u32 size, GF_FilterProbeScore *score)
{
	GF_BitStream *bs = gf_bs_new(data, size, GF_BITSTREAM_READ);
	Bool res;
	u32 lt;
	const char *mime = "video/av1";
	lt = gf_log_get_tool_level(GF_LOG_CODING);
	gf_log_set_tool_level(GF_LOG_CODING, GF_LOG_QUIET);

	res = gf_media_probe_ivf(bs);
	if (res) {
		*score = GF_FPROBE_SUPPORTED;
		mime = "video/x-ivf";
	} else {
		res = gf_media_aom_probe_annexb(bs);
		if (res) *score = GF_FPROBE_SUPPORTED;
		else {
			AV1State state;
			GF_Err e;
			u32 nb_units = 0;

			gf_av1_init_state(&state);
			state.config = gf_odf_av1_cfg_new();
			while (gf_bs_available(bs)) {
				e = aom_av1_parse_temporal_unit_from_section5(bs, &state);
				if ((e==GF_OK) || (nb_units && (e==GF_BUFFER_TOO_SMALL) ) ) {
					if (!nb_units || gf_list_count(state.frame_state.header_obus) || gf_list_count(state.frame_state.frame_obus)) {
						nb_units++;
						if (e==GF_BUFFER_TOO_SMALL)
							nb_units++;
					} else
						break;
				} else {
					break;
				}
				gf_av1_reset_state(&state, GF_FALSE);
				if (nb_units>2) break;
			}
			gf_odf_av1_cfg_del(state.config);
			gf_av1_reset_state(&state, GF_TRUE);
			if (nb_units>2) {
				res = GF_TRUE;
				*score = GF_FPROBE_MAYBE_SUPPORTED;
			}
		}
	}

	gf_log_set_tool_level(GF_LOG_CODING, lt);

	gf_bs_del(bs);
	if (res) return mime;
	return NULL;
}