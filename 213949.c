}

void dump_vvc_track_info(GF_ISOFile *file, u32 trackNum, GF_VVCConfig *vvccfg
#if !defined(GPAC_DISABLE_AV_PARSERS)
	, VVCState *vvc_state
#endif /*GPAC_DISABLE_AV_PARSERS  && defined(GPAC_DISABLE_HEVC)*/
	)
{
#if !defined(GPAC_DISABLE_AV_PARSERS)
	u32 idx;
#endif
	u32 k;
	fprintf(stderr, "\tVVC Info:");

	fprintf(stderr, " Profile %d @ Level %d - Chroma Format %s\n", vvccfg->general_profile_idc, vvccfg->general_level_idc, vvccfg->chromaformat_plus_one ? gf_avc_hevc_get_chroma_format_name(vvccfg->chromaformat_plus_one-1) : "n/a");
	fprintf(stderr, "\n");
	fprintf(stderr, "\tNAL Unit length bits: %d", 8*vvccfg->nal_unit_size);
	if (vvccfg->general_constraint_info && vvccfg->num_constraint_info && vvccfg->general_constraint_info[0]) {
		fprintf(stderr, " - general constraint info 0x");
		for (idx=0; idx<vvccfg->num_constraint_info; idx++) {
			fprintf(stderr, "%02X", vvccfg->general_constraint_info[idx]);
		}
	}
	fprintf(stderr, "\n");
	fprintf(stderr, "\tParameter Sets: ");
	for (k=0; k<gf_list_count(vvccfg->param_array); k++) {
		GF_NALUFFParamArray *ar=gf_list_get(vvccfg->param_array, k);
		if (ar->type==GF_VVC_NALU_SEQ_PARAM) {
			fprintf(stderr, "%d SPS ", gf_list_count(ar->nalus));
		}
		if (ar->type==GF_VVC_NALU_PIC_PARAM) {
			fprintf(stderr, "%d PPS ", gf_list_count(ar->nalus));
		}
		if (ar->type==GF_VVC_NALU_VID_PARAM) {
			fprintf(stderr, "%d VPS ", gf_list_count(ar->nalus));

#if !defined(GPAC_DISABLE_AV_PARSERS) && 0 //TODO
			for (idx=0; idx<gf_list_count(ar->nalus); idx++) {
				GF_NALUFFParam *vps = gf_list_get(ar->nalus, idx);
				s32 ps_idx=gf_hevc_read_vps(vps->data, vps->size, hevc_state);
				if (hevccfg->is_lhvc && (ps_idx>=0)) {
					non_hevc_base_layer = ! hevc_state->vps[ps_idx].base_layer_internal_flag;
				}
			}
#endif

		}
	}

	fprintf(stderr, "\n");
#if !defined(GPAC_DISABLE_AV_PARSERS) && 0 //TODO
	for (k=0; k<gf_list_count(vvccfg->param_array); k++) {
		GF_NALUFFParamArray *ar=gf_list_get(vvccfg->param_array, k);
		u32 width, height;
		s32 par_n, par_d;

		if (ar->type !=GF_VVC_NALU_SEQ_PARAM) continue;
		for (idx=0; idx<gf_list_count(ar->nalus); idx++) {
			GF_Err e;
			GF_NALUFFParam *sps = gf_list_get(ar->nalus, idx);
			par_n = par_d = -1;
			e = gf_vvc_get_sps_info_with_state(vvc_state, sps->data, sps->size, NULL, &width, &height, &par_n, &par_d);
			if (e==GF_OK) {
				fprintf(stderr, "\tSPS resolution %dx%d", width, height);
				if ((par_n>0) && (par_d>0)) {
					u32 tw, th;
					gf_isom_get_track_layout_info(file, trackNum, &tw, &th, NULL, NULL, NULL);
					fprintf(stderr, " - Pixel Aspect Ratio %d:%d - Indicated track size %d x %d", par_n, par_d, tw, th);
				}
				fprintf(stderr, "\n");
			} else {
				M4_LOG(GF_LOG_ERROR, ("\nFailed to read SPS: %s\n\n", gf_error_to_string(e) ));
			}
		}
	}
#endif
	fprintf(stderr, "\tBit Depth %d - %d temporal layers\n", vvccfg->bit_depth_plus_one-1, vvccfg->numTemporalLayers);

	for (k=0; k<gf_list_count(vvccfg->param_array); k++) {
		GF_NALUFFParamArray *ar=gf_list_get(vvccfg->param_array, k);
		if (ar->type==GF_VVC_NALU_SEQ_PARAM) print_config_hash(ar->nalus, "SPS");
		else if (ar->type==GF_VVC_NALU_PIC_PARAM) print_config_hash(ar->nalus, "PPS");
		else if (ar->type==GF_VVC_NALU_VID_PARAM) print_config_hash(ar->nalus, "VPS");