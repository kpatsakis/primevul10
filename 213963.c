

GF_Err dump_isom_chapters(GF_ISOFile *file, char *inName, Bool is_final_name, u32 dump_mode)
{
	FILE *t;
	u32 i, count;
	u32 chap_tk = 0;
	count = gf_isom_get_chapter_count(file, 0);

	if (dump_mode==2) dump_mode = GF_TEXTDUMPTYPE_OGG_CHAP;
	else if (dump_mode==3) dump_mode = GF_TEXTDUMPTYPE_ZOOM_CHAP;
	else dump_mode = GF_TEXTDUMPTYPE_TTXT_CHAP;

	if (!count) {
		for (i=0; i<gf_isom_get_track_count(file); i++) {
			if (gf_isom_get_reference_count(file, i+1, GF_ISOM_REF_CHAP)) {
				GF_Err e = gf_isom_get_reference(file, i+1, GF_ISOM_REF_CHAP, 1, &chap_tk);
				if (!e) break;
			}
		}
		if (!chap_tk) {
			M4_LOG(GF_LOG_WARNING, ("No chapters or chapters track found in file\n"));
			return GF_OK;
		}

		fprintf(stderr, "Dumping chapter track %d\n", chap_tk);
		dump_isom_timed_text(file, gf_isom_get_track_id(file, chap_tk), inName, is_final_name, GF_FALSE, dump_mode);
		return GF_OK;

	}
	if (inName) {
		char szName[1024];
		strcpy(szName, inName);
		if (!is_final_name) {
			if (dump_mode==GF_TEXTDUMPTYPE_OGG_CHAP) {
				strcat(szName, ".txt");
			} else if (dump_mode==GF_TEXTDUMPTYPE_ZOOM_CHAP) {
				strcat(szName, ".txt");
			} else {
				strcat(szName, ".ttxt");
			}
		}
		t = gf_fopen(szName, "wt");
		if (!t) return GF_IO_ERR;
	} else {
		t = stdout;
	}

	if (dump_mode==GF_TEXTDUMPTYPE_TTXT_CHAP) {
		fprintf(t, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
		fprintf(t, "<TextStream version=\"1.1\">\n");
		fprintf(t, "<TextStreamHeader width=\"0\" height=\"0\" layer=\"0\" translation_x=\"0\" translation_y=\"0\">\n");
		fprintf(t, "<TextSampleDescription horizontalJustification=\"left\" backColor=\"0 0 0\" scroll=\"None\"/>\n");
		fprintf(t, "</TextStreamHeader>\n");
	}

	for (i=0; i<count; i++) {
		char szDur[20];
		u64 chapter_time;
		const char *name;
		gf_isom_get_chapter(file, 0, i+1, &chapter_time, &name);
		if (dump_mode==GF_TEXTDUMPTYPE_OGG_CHAP) {
			fprintf(t, "CHAPTER%02d=%s\n", i+1, format_duration(chapter_time, 1000, szDur));
			fprintf(t, "CHAPTER%02dNAME=%s\n", i+1, name);
		} else if (dump_mode==GF_TEXTDUMPTYPE_ZOOM_CHAP) {
			chapter_time /= 1000;
			fprintf(t, "AddChapterBySecond("LLD",%s)\n", chapter_time, name);
		} else {
			fprintf(t, "<TextSample sampleTime=\"%s\" sampleDescriptionIndex=\"1\" xml:space=\"preserve\">%s</TextSample>\n"
				, format_duration(chapter_time, 1000, szDur), name);
		}
	}
	if (dump_mode==GF_TEXTDUMPTYPE_TTXT_CHAP) {
		fprintf(t, "</TextStream>\n");
	}
	if (inName) gf_fclose(t);