}

static void dump_qt_prores(GF_ISOFile *file, u32 trackID, FILE *dump, Bool dump_crc)
{
#ifndef GPAC_DISABLE_AV_PARSERS
	u32 i, count, track, timescale;

	track = gf_isom_get_track_by_id(file, trackID);

	count = gf_isom_get_sample_count(file, track);
	timescale = gf_isom_get_media_timescale(file, track);

	fprintf(dump, "<ProResTrack trackID=\"%d\" SampleCount=\"%d\" TimeScale=\"%d\">\n", trackID, count, timescale);

	for (i=0; i<count; i++) {
		void gf_inspect_dump_prores(FILE *dump, u8 *ptr, u64 frame_size, Bool dump_crc);
		u64 dts, cts;
		GF_ISOSample *samp = gf_isom_get_sample(file, track, i+1, NULL);
		if (!samp) {
			fprintf(dump, "<!-- Unable to fetch sample %d -->\n", i+1);
			continue;
		}
		dts = samp->DTS;
		cts = dts + (s32) samp->CTS_Offset;

		if (cts!=dts) fprintf(dump, "<!-- Wrong timing info (CTS "LLD" vs DTS "LLD") ! -->\n", cts, dts);
		if (!samp->IsRAP) fprintf(dump, "<!-- Wrong sync sample info, sample is not SAP1 ! -->\n");

		fprintf(dump, "  <Sample number=\"%d\" CTS=\""LLD"\" size=\"%d\">\n", i+1, cts, samp->dataLength);

		gf_inspect_dump_prores(dump, samp->data, samp->dataLength, dump_crc);
		fprintf(dump, "  </Sample>\n");

		gf_isom_sample_del(&samp);

		fprintf(dump, "\n");
		gf_set_progress("Analysing ProRes Track", i+1, count);
	}
	fprintf(dump, "</ProResTrack>\n");