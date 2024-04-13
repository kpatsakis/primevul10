}

void DumpMovieInfo(GF_ISOFile *file)
{
	GF_InitialObjectDescriptor *iod;
	Bool dump_m4sys = GF_FALSE;
	u32 i, brand, min, timescale, count, data_len;
	const u8 *data;
	u64 create, modif;
	Bool has_itags = GF_FALSE;
	char szDur[50];

	DumpMetaItem(file, 1, 0, "# File Meta");
	if (!gf_isom_has_movie(file)) {
		if (gf_isom_has_segment(file, &brand, &min)) {
			count = gf_isom_segment_get_fragment_count(file);
			fprintf(stderr, "File is a segment - %d movie fragments - Brand %s (version %d):\n", count, gf_4cc_to_str(brand), min);
			for (i=0; i<count; i++) {
				u32 j, traf_count = gf_isom_segment_get_track_fragment_count(file, i+1);
				for (j=0; j<traf_count; j++) {
					u32 ID;
					u64 tfdt;
					ID = gf_isom_segment_get_track_fragment_decode_time(file, i+1, j+1, &tfdt);
					fprintf(stderr, "\tFragment #%d Track ID %d - TFDT "LLU"\n", i+1, ID, tfdt);
				}
			}
		} else {
			fprintf(stderr, "File has no movie (moov) - static data container\n");
		}
		return;
	}

	timescale = gf_isom_get_timescale(file);
	i=gf_isom_get_track_count(file);
	fprintf(stderr, "# Movie Info - %d track%s - TimeScale %d\n", i, i>1 ? "s" : "", timescale);

	modif = gf_isom_get_duration(file);
	create = gf_isom_get_original_duration(file);
	fprintf(stderr, "Duration %s", format_duration(create, timescale, szDur));
	if (create!=modif) {
		fprintf(stderr, " (recomputed %s)", format_duration(modif, timescale, szDur));
	}
	fprintf(stderr, "\n");

#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	if (gf_isom_is_fragmented(file)) {
		fprintf(stderr, "Fragmented: yes - duration %s\n%d fragments - %d SegmentIndexes\n", format_duration(gf_isom_get_fragmented_duration(file), timescale, szDur), gf_isom_get_fragments_count(file, 0) , gf_isom_get_fragments_count(file, 1) );
	} else {
		fprintf(stderr, "Fragmented: no\n");
	}
#endif

	if (gf_isom_moov_first(file))
		fprintf(stderr, "Progressive (moov before mdat)\n");

	if (gf_isom_get_brand_info(file, &brand, &min, &count) == GF_OK) {
		fprintf(stderr, "Major Brand %s - version %d - compatible brands:", gf_4cc_to_str(brand), min);
		for (i=0; i<count;i++) {
			if (gf_isom_get_alternate_brand(file, i+1, &brand)==GF_OK)
				fprintf(stderr, " %s", gf_4cc_to_str(brand) );
		}
		fprintf(stderr, "\n");
	}
	gf_isom_get_creation_time(file, &create, &modif);
	fprintf(stderr, "Created: %s", format_date(create, szDur));
	if (create != modif)
		fprintf(stderr, "Modified: %s", format_date(modif, szDur));
	fprintf(stderr, "\n");

	DumpMetaItem(file, 0, 0, "# Movie Meta");

	iod = (GF_InitialObjectDescriptor *) gf_isom_get_root_od(file);
	if (iod) {
		u32 desc_size = gf_odf_desc_size((GF_Descriptor *)iod);
		if (iod->tag == GF_ODF_IOD_TAG) {
			fprintf(stderr, "File has root IOD (%d bytes)\n", desc_size);
			fprintf(stderr, "Scene PL 0x%02x - Graphics PL 0x%02x - OD PL 0x%02x\n", iod->scene_profileAndLevel, iod->graphics_profileAndLevel, iod->OD_profileAndLevel);
			fprintf(stderr, "Visual PL: %s (0x%02x)\n", gf_m4v_get_profile_name(iod->visual_profileAndLevel), iod->visual_profileAndLevel);
			fprintf(stderr, "Audio PL: %s (0x%02x)\n", gf_m4a_get_profile_name(iod->audio_profileAndLevel), iod->audio_profileAndLevel);
			//fprintf(stderr, "inline profiles included %s\n", iod->inlineProfileFlag ? "yes" : "no");
		} else {
			fprintf(stderr, "File has root OD (%d bytes)\n", desc_size);
		}
		if (!gf_list_count(iod->ESDescriptors))
			fprintf(stderr, "No streams included in root OD\n");
		else
			dump_m4sys = GF_TRUE;

		gf_odf_desc_del((GF_Descriptor *) iod);
	}
	if (gf_isom_is_JPEG2000(file)) fprintf(stderr, "File is JPEG 2000\n");

	count = gf_isom_get_copyright_count(file);
	if (count) {
		const char *lang, *note;
		fprintf(stderr, "\nCopyrights:\n");
		for (i=0; i<count; i++) {
			gf_isom_get_copyright(file, i+1, &lang, &note);
			fprintf(stderr, "\t(%s) %s\n", lang, note);
		}
	}

	count = gf_isom_get_chapter_count(file, 0);
	if (count) {
		const char *name;
		u64 time;
		fprintf(stderr, "\nChapters:\n");
		for (i=0; i<count; i++) {
			gf_isom_get_chapter(file, 0, i+1, &time, &name);
			fprintf(stderr, "\tChapter #%d - %s - \"%s\"\n", i+1, format_duration(time, 1000, szDur), name);
		}
	}

	if (gf_isom_apple_get_tag(file, 0, &data, &data_len) == GF_OK) {
		has_itags = GF_TRUE;
		fprintf(stderr, "\niTunes Info:\n");

		i=0;
		while (1) {
			u32 int_val2, flags, itype;
			GF_ISOiTunesTag tag;
			u64 int_val;
			s32 tag_idx;
			GF_Err e = gf_isom_apple_enum_tag(file, i, &tag, &data, &data_len, &int_val, &int_val2, &flags);
			if (e) break;
			i++;

			tag_idx = gf_itags_find_by_itag(tag);
			if (tag_idx<0) {
				fprintf(stderr, "\t%s: %s\n", gf_4cc_to_str(tag), data);
				continue;
			}
			fprintf(stderr, "\t%s: ", gf_itags_get_name(tag_idx) );
			itype = gf_itags_get_type(tag_idx);
			switch (itype) {
			case GF_ITAG_BOOL: fprintf(stderr, int_val ? "yes" : "no"); break;
			case GF_ITAG_INT8:
			case GF_ITAG_INT16:
			case GF_ITAG_INT32:
			case GF_ITAG_INT64:
				fprintf(stderr, LLU, int_val);
				break;
			case GF_ITAG_FRAC6:
			case GF_ITAG_FRAC8:
				fprintf(stderr, LLU" / %u", int_val, int_val2);
				break;
			case GF_ITAG_FILE:
				if (flags==14) fprintf(stderr, "PNG File");
				else if (flags==13) fprintf(stderr, "JPEG File");
				else fprintf(stderr, "unknown (flags %d)", flags);
				break;
			case GF_ITAG_ID3_GENRE:
				if (int_val) {
					fprintf(stderr, "%s", gf_id3_get_genre((u32) int_val) );
					break;
				}
				//fallthrough
			default:
				if (data)
					fprintf(stderr, "%s", data);
				else
					fprintf(stderr, data_len ? "none" : "unknown");
				break;
			}
			fprintf(stderr, "\n");
		}
	}
	i=0;
	while (1) {
		u32 type, version;
		char *wmatag;
		GF_Err e = gf_isom_wma_enum_tag(file, i, &wmatag, &data, &data_len, &version, &type);
		if (e) break;
		if (!i) {
			fprintf(stderr, "\nWMA Info:\n");
		}
		i++;
		fprintf(stderr, "\t%s", wmatag);
		if (version!=1)
			fprintf(stderr, " (version %d)", version);
		fprintf(stderr, ": ");

		if (type) {
			fprintf(stderr, "unknown type %d\n", type);
		} else {
			u16 *src_str = (u16 *) data;
			u32 len = (u32) ( UTF8_MAX_BYTES_PER_CHAR * gf_utf8_wcslen(src_str) );
			char *utf8str = (char *)gf_malloc(len + 1);
			u32 res_len = (u32) gf_utf8_wcstombs(utf8str, len, (const unsigned short **) &src_str);
			utf8str[res_len] = 0;
			fprintf(stderr, "%s\n", utf8str);

			gf_free(utf8str);
		}
	}


	print_udta(file, 0, has_itags);
	fprintf(stderr, "\n");
	for (i=0; i<gf_isom_get_track_count(file); i++) {
		DumpTrackInfo(file, i+1, 0, GF_TRUE, dump_m4sys);