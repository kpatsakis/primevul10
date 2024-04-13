}

void dump_isom_saps(GF_ISOFile *file, GF_ISOTrackID trackID, u32 dump_saps_mode, char *inName, Bool is_final_name)
{
	FILE *dump;
	u32 i, count;
	s64 media_offset=0;
	u32 track = gf_isom_get_track_by_id(file, trackID);
	if (inName) {
		char szBuf[GF_MAX_PATH];
		strcpy(szBuf, inName);

		if (!is_final_name) sprintf(szBuf, "%s_%d_cues.xml", inName, trackID);
		dump = gf_fopen(szBuf, "wt");
		if (!dump) {
			fprintf(stderr, "Failed to open %s for dumping\n", szBuf);
			return;
		}
	} else {
		dump = stdout;
	}

	fprintf(dump, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	fprintf(dump, "<DASHCues xmlns=\"urn:gpac:dash:schema:cues:2018\">\n");
	fprintf(dump, "<Stream id=\"%d\" timescale=\"%d\"", trackID, gf_isom_get_media_timescale(file, track) );
	if (dump_saps_mode==4) {
		fprintf(dump, " mode=\"edit\"");
		gf_isom_get_edit_list_type(file, track, &media_offset);
	}
	fprintf(dump, ">\n");

	count = gf_isom_get_sample_count(file, track);
	for (i=0; i<count; i++) {
		s64 cts, dts;
		u32 di;
		Bool traf_start = 0;
		u32 sap_type = 0;
		u64 doffset;
		
		GF_ISOSample *samp = gf_isom_get_sample_info(file, track, i+1, &di, &doffset);

		traf_start = gf_isom_sample_is_fragment_start(file, track, i+1, NULL);

		sap_type = samp->IsRAP;
		if (!sap_type) {
			Bool is_rap;
			GF_ISOSampleRollType roll_type;
			s32 roll_dist;
			gf_isom_get_sample_rap_roll_info(file, track, i+1, &is_rap, &roll_type, &roll_dist);
			if (roll_type) sap_type = SAP_TYPE_4;
			else if (is_rap)  sap_type = SAP_TYPE_3;
		}

		if (!sap_type) {
			gf_isom_sample_del(&samp);
			continue;
		}

		dts = cts = samp->DTS;
		cts += samp->CTS_Offset;
		fprintf(dump, "<Cue sap=\"%d\"", sap_type);
		if (dump_saps_mode==4) {
			cts += media_offset;
			fprintf(dump, " cts=\""LLD"\"", cts);
		} else {
			if (!dump_saps_mode || (dump_saps_mode==1)) fprintf(dump, " sample=\"%d\"", i+1);
			if (!dump_saps_mode || (dump_saps_mode==2)) fprintf(dump, " cts=\""LLD"\"", cts);
			if (!dump_saps_mode || (dump_saps_mode==3)) fprintf(dump, " dts=\""LLD"\"", dts);
		}

		if (traf_start)
			fprintf(dump, " wasFragStart=\"yes\"");

		fprintf(dump, "/>\n");

		gf_isom_sample_del(&samp);
	}
	fprintf(dump, "</Stream>\n");
	fprintf(dump, "</DASHCues>\n");