static void dump_isom_nal_ex(GF_ISOFile *file, GF_ISOTrackID trackID, FILE *dump, u32 dump_flags)
{
	u32 i, j, count, nb_descs, track, nalh_size, timescale, cur_extract_mode;
	s32 countRef;
	Bool is_adobe_protected = GF_FALSE;
	Bool is_cenc_protected = GF_FALSE;
	Bool is_hevc = GF_FALSE;
#ifndef GPAC_DISABLE_AV_PARSERS
	AVCState avc;
	HEVCState hevc;
#endif
	GF_AVCConfig *avccfg, *svccfg;
	GF_HEVCConfig *hevccfg, *lhvccfg;
	GF_AVCConfigSlot *slc;
	Bool has_svcc = GF_FALSE;

	track = gf_isom_get_track_by_id(file, trackID);

#ifndef GPAC_DISABLE_AV_PARSERS
	memset(&avc, 0, sizeof(AVCState));
	memset(&hevc, 0, sizeof(HEVCState));
#endif

	count = gf_isom_get_sample_count(file, track);

	timescale = gf_isom_get_media_timescale(file, track);

	cur_extract_mode = gf_isom_get_nalu_extract_mode(file, track);

	nb_descs = gf_isom_get_sample_description_count(file, track);
	if (!nb_descs) {
		fprintf(stderr, "Error: Track #%d has no sample description so is likely not NALU-based!\n", trackID);
		return;
	}

	fprintf(dump, "<NALUTrack trackID=\"%d\" SampleCount=\"%d\" TimeScale=\"%d\">\n", trackID, count, timescale);

#ifndef GPAC_DISABLE_AV_PARSERS

#define DUMP_ARRAY(arr, name, loc, _is_svc)\
	if (arr) {\
		fprintf(dump, "  <%sArray location=\"%s\">\n", name, loc);\
		for (i=0; i<gf_list_count(arr); i++) {\
			slc = gf_list_get(arr, i);\
			fprintf(dump, "   <NALU size=\"%d\" ", slc->size);\
			gf_inspect_dump_nalu(dump, (u8 *) slc->data, slc->size, _is_svc, is_hevc ? &hevc : NULL, &avc, nalh_size, (dump_flags&1) ? GF_TRUE : GF_FALSE, GF_FALSE);\
			fprintf(dump, "/>\n");\
		}\
		fprintf(dump, "  </%sArray>\n", name);\
	}\

#else

#define DUMP_ARRAY(arr, name, loc, _is_svc)\
	if (arr) {\
		fprintf(dump, "  <%sArray location=\"%s\">\n", name, loc);\
		for (i=0; i<gf_list_count(arr); i++) {\
			slc = gf_list_get(arr, i);\
			fprintf(dump, "   <NALU size=\"%d\" ", slc->size);\
			fprintf(dump, "/>\n");\
		}\
		fprintf(dump, "  </%sArray>\n", name);\
	}\

#endif

	nalh_size = 0;

	for (j=0; j<nb_descs; j++) {
		GF_AVCConfig *mvccfg;
		Bool is_svc;

		avccfg = gf_isom_avc_config_get(file, track, j+1);
		svccfg = gf_isom_svc_config_get(file, track, j+1);
		mvccfg = gf_isom_mvc_config_get(file, track, j+1);
		hevccfg = gf_isom_hevc_config_get(file, track, j+1);
		lhvccfg = gf_isom_lhvc_config_get(file, track, j+1);

		is_svc = (svccfg!=NULL) ? 1:0;

		//for tile tracks the hvcC is stored in the 'tbas' track
		if (!hevccfg && gf_isom_get_reference_count(file, track, GF_ISOM_REF_TBAS)) {
			u32 tk = 0;
			gf_isom_get_reference(file, track, GF_ISOM_REF_TBAS, 1, &tk);
			hevccfg = gf_isom_hevc_config_get(file, tk, 1);
		}

		fprintf(dump, " <NALUConfig>\n");

		if (!avccfg && !svccfg && !hevccfg && !lhvccfg) {
			fprintf(stderr, "Error: Track #%d is not NALU-based!\n", trackID);
			return;
		}

		if (avccfg) {
			nalh_size = avccfg->nal_unit_size;

			DUMP_ARRAY(avccfg->sequenceParameterSets, "AVCSPS", "avcC", is_svc);
			DUMP_ARRAY(avccfg->pictureParameterSets, "AVCPPS", "avcC", is_svc)
			DUMP_ARRAY(avccfg->sequenceParameterSetExtensions, "AVCSPSEx", "avcC", is_svc)
		}
		if (is_svc) {
			if (!nalh_size) nalh_size = svccfg->nal_unit_size;
			DUMP_ARRAY(svccfg->sequenceParameterSets, "SVCSPS", "svcC", is_svc)
			DUMP_ARRAY(svccfg->pictureParameterSets, "SVCPPS", "svcC", is_svc)
		}
		if (mvccfg) {
			if (!nalh_size) nalh_size = mvccfg->nal_unit_size;
			DUMP_ARRAY(mvccfg->sequenceParameterSets, "SVCSPS", "mvcC", is_svc)
			DUMP_ARRAY(mvccfg->pictureParameterSets, "SVCPPS", "mvcC", is_svc)
		}
		if (hevccfg) {
			u32 idx;
			nalh_size = hevccfg->nal_unit_size;
			is_hevc = 1;
			for (idx=0; idx<gf_list_count(hevccfg->param_array); idx++) {
				GF_HEVCParamArray *ar = gf_list_get(hevccfg->param_array, idx);
				if (ar->type==GF_HEVC_NALU_SEQ_PARAM) {
					DUMP_ARRAY(ar->nalus, "HEVCSPS", "hvcC", 0)
				} else if (ar->type==GF_HEVC_NALU_PIC_PARAM) {
					DUMP_ARRAY(ar->nalus, "HEVCPPS", "hvcC", 0)
				} else if (ar->type==GF_HEVC_NALU_VID_PARAM) {
					DUMP_ARRAY(ar->nalus, "HEVCVPS", "hvcC", 0)
				} else {
					DUMP_ARRAY(ar->nalus, "HEVCUnknownPS", "hvcC", 0)
				}
			}
		}
		if (lhvccfg) {
			u32 idx;
			nalh_size = lhvccfg->nal_unit_size;
			is_hevc = 1;
			for (idx=0; idx<gf_list_count(lhvccfg->param_array); idx++) {
				GF_HEVCParamArray *ar = gf_list_get(lhvccfg->param_array, idx);
				if (ar->type==GF_HEVC_NALU_SEQ_PARAM) {
					DUMP_ARRAY(ar->nalus, "HEVCSPS", "lhcC", 0)
				} else if (ar->type==GF_HEVC_NALU_PIC_PARAM) {
					DUMP_ARRAY(ar->nalus, "HEVCPPS", "lhcC", 0)
				} else if (ar->type==GF_HEVC_NALU_VID_PARAM) {
					DUMP_ARRAY(ar->nalus, "HEVCVPS", "lhcC", 0)
				} else {
					DUMP_ARRAY(ar->nalus, "HEVCUnknownPS", "lhcC", 0)
				}
			}
		}
		fprintf(dump, " </NALUConfig>\n");

		if (avccfg) gf_odf_avc_cfg_del(avccfg);
		if (svccfg) {
			gf_odf_avc_cfg_del(svccfg);
			has_svcc = GF_TRUE;
		}
		if (hevccfg) gf_odf_hevc_cfg_del(hevccfg);
		if (lhvccfg) gf_odf_hevc_cfg_del(lhvccfg);
	}

	/*fixme: for dumping encrypted track: we don't have neither avccfg nor svccfg*/
	if (!nalh_size) nalh_size = 4;

	/*for testing dependency*/
	countRef = gf_isom_get_reference_count(file, track, GF_ISOM_REF_SCAL);
	if (countRef > 0)
	{
		GF_ISOTrackID refTrackID;
		fprintf(dump, " <SCALReferences>\n");
		for (i = 1; i <= (u32) countRef; i++)
		{
			gf_isom_get_reference_ID(file, track, GF_ISOM_REF_SCAL, i, &refTrackID);
			fprintf(dump, "  <SCALReference number=\"%d\" refTrackID=\"%d\"/>\n", i, refTrackID);
		}

		fprintf(dump, " </SCALReferences>\n");
	}

	fprintf(dump, " <NALUSamples>\n");
	gf_isom_set_nalu_extract_mode(file, track, GF_ISOM_NALU_EXTRACT_INSPECT);
	is_adobe_protected = gf_isom_is_adobe_protection_media(file, track, 1);
	is_cenc_protected = gf_isom_is_cenc_media(file, track, 1);
	for (i=0; i<count; i++) {
		u64 dts, cts;
		Bool is_rap;
		u32 size, nal_size, idx, di;
		u8 *ptr;
		GF_ISOSample *samp = gf_isom_get_sample(file, track, i+1, &di);
		if (!samp) {
			fprintf(dump, "<!-- Unable to fetch sample %d -->\n", i+1);
			continue;
		}
		dts = samp->DTS;
		cts = dts + (s32) samp->CTS_Offset;
		is_rap = samp->IsRAP;
		if (!is_rap) gf_isom_get_sample_rap_roll_info(file, track, i+1, &is_rap, NULL, NULL);

		if (dump_flags&2) {
			fprintf(dump, "  <Sample size=\"%d\" RAP=\"%d\"", samp->dataLength, is_rap);
		} else {
			fprintf(dump, "  <Sample DTS=\""LLD"\" CTS=\""LLD"\" size=\"%d\" RAP=\"%d\"", dts, cts, samp->dataLength, is_rap);
		}
		if (nb_descs>1)
			fprintf(dump, " sample_description=\"%d\"", di);
		fprintf(dump, " >\n");

		if (cts<dts) fprintf(dump, "<!-- NEGATIVE CTS OFFSET! -->\n");

		idx = 1;
		ptr = samp->data;
		size = samp->dataLength;
		if (is_adobe_protected) {
			u8 encrypted_au = ptr[0];
			if (encrypted_au) {
				fprintf(dump, "   <!-- Sample number %d is an Adobe's protected sample: can not be dumped -->\n", i+1);
				fprintf(dump, "  </Sample>\n\n");
				continue;
			}
			else {
				ptr++;
				size--;
			}
		}
		while (size) {
			nal_size = read_nal_size_hdr(ptr, nalh_size);
			ptr += nalh_size;

			if (nal_size >= UINT_MAX-nalh_size || nalh_size + nal_size > size) {
				fprintf(dump, "   <!-- NALU number %d is corrupted: size is %d but only %d remains -->\n", idx, nal_size, size);
				break;
			} else {
				fprintf(dump, "   <NALU size=\"%d\" ", nal_size);
#ifndef GPAC_DISABLE_AV_PARSERS
				Bool is_encrypted = 0;
				if (is_cenc_protected) {
					GF_Err e = gf_isom_get_sample_cenc_info(file, track, i + 1, &is_encrypted, NULL, NULL, NULL, NULL, NULL, NULL);
					if (e != GF_OK) {
						fprintf(dump, "dump_msg=\"Error %s while fetching encryption info for sample, assuming sample is encrypted\" ", gf_error_to_string(e) );
						is_encrypted = GF_TRUE;
					}
				}
				gf_inspect_dump_nalu(dump, ptr, nal_size, has_svcc ? 1 : 0, is_hevc ? &hevc : NULL, &avc, nalh_size, dump_flags, is_encrypted);
#endif
				fprintf(dump, "/>\n");
			}
			idx++;
			ptr+=nal_size;
			size -= nal_size + nalh_size;
		}
		fprintf(dump, "  </Sample>\n");
		gf_isom_sample_del(&samp);

		fprintf(dump, "\n");
		gf_set_progress("Analysing Track NALUs", i+1, count);
	}
	fprintf(dump, " </NALUSamples>\n");
	fprintf(dump, "</NALUTrack>\n");
