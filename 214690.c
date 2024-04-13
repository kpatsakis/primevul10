

void dump_isom_timed_text(GF_ISOFile *file, GF_ISOTrackID trackID, char *inName, Bool is_final_name, Bool is_convert, GF_TextDumpType dump_type)
{
	FILE *dump;
	GF_Err e;
	u32 track;

	track = gf_isom_get_track_by_id(file, trackID);
	if (!track) {
		fprintf(stderr, "Cannot find track ID %d\n", trackID);
		return;
	}

	switch (gf_isom_get_media_type(file, track)) {
	case GF_ISOM_MEDIA_TEXT:
	case GF_ISOM_MEDIA_SUBT:
		break;
	default:
		fprintf(stderr, "Track ID %d is not a 3GPP text track\n", trackID);
		return;
	}

	if (inName) {
		char szBuf[1024];
		char *ext;
		ext = ((dump_type==GF_TEXTDUMPTYPE_SVG) ? "svg" : ((dump_type==GF_TEXTDUMPTYPE_SRT) ? "srt" : "ttxt"));
		if (is_final_name) {
			strcpy(szBuf, inName) ;
		} else if (is_convert)
			sprintf(szBuf, "%s.%s", inName, ext) ;
		else
			sprintf(szBuf, "%s_%d_text.%s", inName, trackID, ext);

		dump = gf_fopen(szBuf, "wt");
		if (!dump) {
			fprintf(stderr, "Failed to open %s for dumping\n", szBuf);
			return;
		}
	} else {
		dump = stdout;
	}
	e = gf_isom_text_dump(file, track, dump, dump_type);
	if (inName) gf_fclose(dump);

	if (e) fprintf(stderr, "Conversion failed (%s)\n", gf_error_to_string(e));