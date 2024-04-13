}

void dump_hevc_track_info(GF_ISOFile *file, u32 trackNum, GF_HEVCConfig *hevccfg
#if !defined(GPAC_DISABLE_AV_PARSERS) && !defined(GPAC_DISABLE_HEVC)
	, HEVCState *hevc_state
#endif /*GPAC_DISABLE_AV_PARSERS  && defined(GPAC_DISABLE_HEVC)*/
	)
{
#if !defined(GPAC_DISABLE_AV_PARSERS) && !defined(GPAC_DISABLE_HEVC)
	u32 idx;
#endif
	u32 k;
	Bool non_hevc_base_layer=GF_FALSE;
	fprintf(stderr, "\t%s Info:", hevccfg->is_lhvc ? "LHVC" : "HEVC");
	if (!hevccfg->is_lhvc)
		fprintf(stderr, " Profile %s @ Level %g - Chroma Format %s\n", gf_hevc_get_profile_name(hevccfg->profile_idc), ((Double)hevccfg->level_idc) / 30.0, gf_avc_hevc_get_chroma_format_name(hevccfg->chromaFormat));
	fprintf(stderr, "\n");
	fprintf(stderr, "\tNAL Unit length bits: %d", 8*hevccfg->nal_unit_size);
	if (!hevccfg->is_lhvc)
		fprintf(stderr, " - general profile compatibility 0x%08X\n", hevccfg->general_profile_compatibility_flags);
	fprintf(stderr, "\n");
	fprintf(stderr, "\tParameter Sets: ");
	for (k=0; k<gf_list_count(hevccfg->param_array); k++) {
		GF_NALUFFParamArray *ar=gf_list_get(hevccfg->param_array, k);
		if (ar->type==GF_HEVC_NALU_SEQ_PARAM) {
			fprintf(stderr, "%d SPS ", gf_list_count(ar->nalus));
		}
		if (ar->type==GF_HEVC_NALU_PIC_PARAM) {
			fprintf(stderr, "%d PPS ", gf_list_count(ar->nalus));
		}
		if (ar->type==GF_HEVC_NALU_VID_PARAM) {
			fprintf(stderr, "%d VPS ", gf_list_count(ar->nalus));

#if !defined(GPAC_DISABLE_AV_PARSERS) && !defined(GPAC_DISABLE_HEVC)
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
#if !defined(GPAC_DISABLE_AV_PARSERS) && !defined(GPAC_DISABLE_HEVC)
	for (k=0; k<gf_list_count(hevccfg->param_array); k++) {
		GF_NALUFFParamArray *ar=gf_list_get(hevccfg->param_array, k);
		u32 width, height;
		s32 par_n, par_d;

		if (ar->type !=GF_HEVC_NALU_SEQ_PARAM) continue;
		for (idx=0; idx<gf_list_count(ar->nalus); idx++) {
			GF_Err e;
			GF_NALUFFParam *sps = gf_list_get(ar->nalus, idx);
			par_n = par_d = -1;
			e = gf_hevc_get_sps_info_with_state(hevc_state, sps->data, sps->size, NULL, &width, &height, &par_n, &par_d);
			if (e==GF_OK) {
				fprintf(stderr, "\tSPS resolution %dx%d", width, height);
				if ((par_n>0) && (par_d>0)) {
					u32 tw, th;
					gf_isom_get_track_layout_info(file, trackNum, &tw, &th, NULL, NULL, NULL);
					fprintf(stderr, " - Pixel Aspect Ratio %d:%d - Indicated track size %d x %d", par_n, par_d, tw, th);
				}
				fprintf(stderr, "\n");
			} else {
				M4_LOG(GF_LOG_ERROR, ("Failed to read SPS: %s\n\n", gf_error_to_string(e) ));
			}
		}
	}
#endif
	if (!hevccfg->is_lhvc)
		fprintf(stderr, "\tBit Depth luma %d - Chroma %d - %d temporal layers\n", hevccfg->luma_bit_depth, hevccfg->chroma_bit_depth, hevccfg->numTemporalLayers);
	else
		fprintf(stderr, "\t%d temporal layers\n", hevccfg->numTemporalLayers);
	if (hevccfg->is_lhvc) {
		fprintf(stderr, "\t%sHEVC base layer - Complete representation %d\n", non_hevc_base_layer ? "Non-" : "", hevccfg->complete_representation);
	}

	for (k=0; k<gf_list_count(hevccfg->param_array); k++) {
		GF_NALUFFParamArray *ar=gf_list_get(hevccfg->param_array, k);
		if (ar->type==GF_HEVC_NALU_SEQ_PARAM) print_config_hash(ar->nalus, "SPS");
		else if (ar->type==GF_HEVC_NALU_PIC_PARAM) print_config_hash(ar->nalus, "PPS");
		else if (ar->type==GF_HEVC_NALU_VID_PARAM) print_config_hash(ar->nalus, "VPS");