#ifndef GPAC_DISABLE_ISOM_DUMP

GF_Err dump_isom_xml(GF_ISOFile *file, char *inName, Bool is_final_name, Bool do_track_dump, Bool merge_vtt_cues, Bool skip_init, Bool skip_samples)
{
	GF_Err e;
	FILE *dump = stdout;
	Bool do_close=GF_FALSE;
	if (!file) return GF_ISOM_INVALID_FILE;

	if (inName) {
		char szBuf[1024];
		strcpy(szBuf, inName);
		if (!is_final_name) {
			strcat(szBuf, do_track_dump ? "_dump.xml" : "_info.xml");
		}
		dump = gf_fopen(szBuf, "wt");
		if (!dump) {
			M4_LOG(GF_LOG_ERROR, ("Failed to open %s\n", szBuf));
			return GF_IO_ERR;
		}
		do_close=GF_TRUE;
	}

	fprintf(dump, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	if (do_track_dump) {
		fprintf(dump, "<ISOBaseMediaFileTrace>\n");
	}
	e = gf_isom_dump(file, dump, skip_init, skip_samples);
	if (e) {
		M4_LOG(GF_LOG_ERROR, ("Error dumping ISO structure\n"));
	}

	if (do_track_dump) {
#ifndef GPAC_DISABLE_MEDIA_EXPORT
		u32 i;
		//because of dump mode we need to reopen in regular read mode to avoid mem leaks
		GF_ISOFile *the_file = gf_isom_open(gf_isom_get_filename(file), GF_ISOM_OPEN_READ, NULL);
		u32 tcount = gf_isom_get_track_count(the_file);
		fprintf(dump, "<Tracks>\n");

		for (i=0; i<tcount; i++) {
			GF_MediaExporter dumper;
			GF_ISOTrackID trackID = gf_isom_get_track_id(the_file, i+1);
			u32 mtype = gf_isom_get_media_type(the_file, i+1);
			u32 msubtype = gf_isom_get_media_subtype(the_file, i+1, 1);
			Bool fmt_handled = GF_FALSE;
			memset(&dumper, 0, sizeof(GF_MediaExporter));
			dumper.file = the_file;
			dumper.trackID = trackID;
			dumper.dump_file = dump;

			if (mtype == GF_ISOM_MEDIA_HINT) {
#ifndef GPAC_DISABLE_ISOM_HINTING
				char *name=NULL;
				if (msubtype==GF_ISOM_SUBTYPE_RTP) name = "RTPHintTrack";
				else if (msubtype==GF_ISOM_SUBTYPE_SRTP) name = "SRTPHintTrack";
				else if (msubtype==GF_ISOM_SUBTYPE_RRTP) name = "RTPReceptionHintTrack";
				else if (msubtype==GF_ISOM_SUBTYPE_RTCP) name = "RTCPReceptionHintTrack";
				else if (msubtype==GF_ISOM_SUBTYPE_FLUTE) name = "FLUTEReceptionHintTrack";
				else name = "UnknownHintTrack";

				fprintf(dump, "<%s trackID=\"%d\">\n", name, trackID);

#ifndef GPAC_DISABLE_ISOM_HINTING
				u32 j, scount=gf_isom_get_sample_count(the_file, i+1);
				for (j=0; j<scount; j++) {
					gf_isom_dump_hint_sample(the_file, i+1, j+1, dump);
				}
#endif
				fprintf(dump, "</%s>\n", name);
				fmt_handled = GF_TRUE;
#endif /*GPAC_DISABLE_ISOM_HINTING*/
			}
			else if (gf_isom_get_avc_svc_type(the_file, i+1, 1) || gf_isom_get_hevc_lhvc_type(the_file, i+1, 1)) {
				dump_isom_nal_ex(the_file, trackID, dump, GF_FALSE);
				fmt_handled = GF_TRUE;
			} else if ((mtype==GF_ISOM_MEDIA_TEXT) || (mtype==GF_ISOM_MEDIA_SUBT) ) {

				if (msubtype==GF_ISOM_SUBTYPE_WVTT) {
					gf_webvtt_dump_iso_track(&dumper, i+1, merge_vtt_cues, GF_TRUE);
					fmt_handled = GF_TRUE;
				} else if ((msubtype==GF_ISOM_SUBTYPE_TX3G) || (msubtype==GF_ISOM_SUBTYPE_TEXT)) {
					gf_isom_text_dump(the_file, i+1, dump, GF_TEXTDUMPTYPE_TTXT_BOXES);
					fmt_handled = GF_TRUE;
				}
			}

			if (!fmt_handled) {
				dumper.flags = GF_EXPORT_NHML | GF_EXPORT_NHML_FULL;
				dumper.print_stats_graph = fs_dump_flags;
				gf_media_export(&dumper);
			}
		}
#else
		return GF_NOT_SUPPORTED;
#endif /*GPAC_DISABLE_MEDIA_EXPORT*/
		gf_isom_delete(the_file);
		fprintf(dump, "</Tracks>\n");
		fprintf(dump, "</ISOBaseMediaFileTrace>\n");
	}
	if (do_close) gf_fclose(dump);