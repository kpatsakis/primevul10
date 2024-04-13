}

void DumpMovieInfo(GF_ISOFile *file)
{
	GF_InitialObjectDescriptor *iod;
	u32 i, brand, min, timescale, count, tag_len;
	const u8 *tag;
	u64 create, modif;
	char szDur[50];

	DumpMetaItem(file, 1, 0, "Root Meta");
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
	fprintf(stderr, "* Movie Info *\n\tTimescale %d - %d track%s\n", timescale, i, i>1 ? "s" : "");

	fprintf(stderr, "\tComputed Duration %s", format_duration(gf_isom_get_duration(file), timescale, szDur));
	fprintf(stderr, " - Indicated Duration %s\n", format_duration(gf_isom_get_original_duration(file), timescale, szDur));

#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	if (gf_isom_is_fragmented(file)) {
		fprintf(stderr, "\tFragmented File: yes - duration %s\n%d fragments - %d SegmentIndexes\n", format_duration(gf_isom_get_fragmented_duration(file), timescale, szDur), gf_isom_get_fragments_count(file, 0) , gf_isom_get_fragments_count(file, 1) );
	} else {
		fprintf(stderr, "\tFragmented File: no\n");
	}
#endif

	if (gf_isom_moov_first(file))
		fprintf(stderr, "\tFile suitable for progressive download (moov before mdat)\n");

	if (gf_isom_get_brand_info(file, &brand, &min, &count) == GF_OK) {
		fprintf(stderr, "\tFile Brand %s - version %d\n\t\tCompatible brands:", gf_4cc_to_str(brand), min);
		for (i=0; i<count;i++) {
			if (gf_isom_get_alternate_brand(file, i+1, &brand)==GF_OK)
				fprintf(stderr, " %s", gf_4cc_to_str(brand) );
		}
		fprintf(stderr, "\n");
	}
	gf_isom_get_creation_time(file, &create, &modif);
	fprintf(stderr, "\tCreated: %s", format_date(create, szDur));
	fprintf(stderr, "\tModified: %s", format_date(modif, szDur));
	fprintf(stderr, "\n");

	DumpMetaItem(file, 0, 0, "Moov Meta");

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
		if (!gf_list_count(iod->ESDescriptors)) fprintf(stderr, "No streams included in root OD\n");
		gf_odf_desc_del((GF_Descriptor *) iod);
	} else {
		fprintf(stderr, "File has no MPEG4 IOD/OD\n");
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

	if (gf_isom_apple_get_tag(file, 0, &tag, &tag_len) == GF_OK) {
		fprintf(stderr, "\niTunes Info:\n");
		if (gf_isom_apple_get_tag(file, GF_ISOM_ITUNE_NAME, &tag, &tag_len)==GF_OK) fprintf(stderr, "\tName: %s\n", tag);
		if (gf_isom_apple_get_tag(file, GF_ISOM_ITUNE_ARTIST, &tag, &tag_len)==GF_OK) fprintf(stderr, "\tArtist: %s\n", tag);
		if (gf_isom_apple_get_tag(file, GF_ISOM_ITUNE_ALBUM, &tag, &tag_len)==GF_OK) fprintf(stderr, "\tAlbum: %s\n", tag);
		if (gf_isom_apple_get_tag(file, GF_ISOM_ITUNE_COMMENT, &tag, &tag_len)==GF_OK) fprintf(stderr, "\tComment: %s\n", tag);
		if (gf_isom_apple_get_tag(file, GF_ISOM_ITUNE_COMPOSER, &tag, &tag_len)==GF_OK) fprintf(stderr, "\tComposer: %s\n", tag);
		if (gf_isom_apple_get_tag(file, GF_ISOM_ITUNE_WRITER, &tag, &tag_len)==GF_OK) fprintf(stderr, "\tWriter: %s\n", tag);
		if (gf_isom_apple_get_tag(file, GF_ISOM_ITUNE_ALBUM_ARTIST, &tag, &tag_len)==GF_OK) fprintf(stderr, "\tAlbum Artist: %s\n", tag);

		if (gf_isom_apple_get_tag(file, GF_ISOM_ITUNE_GENRE, &tag, &tag_len)==GF_OK) {
			if (tag[0]) {
				fprintf(stderr, "\tGenre: %s\n", tag);
			} else {
				fprintf(stderr, "\tGenre: %s\n", gf_id3_get_genre(((u8*)tag)[1]));
			}
		}
		if (gf_isom_apple_get_tag(file, GF_ISOM_ITUNE_COMPILATION, &tag, &tag_len)==GF_OK) fprintf(stderr, "\tCompilation: %s\n", tag[0] ? "Yes" : "No");
		if (gf_isom_apple_get_tag(file, GF_ISOM_ITUNE_GAPLESS, &tag, &tag_len)==GF_OK) fprintf(stderr, "\tGapless album: %s\n", tag[0] ? "Yes" : "No");

		if (gf_isom_apple_get_tag(file, GF_ISOM_ITUNE_CREATED, &tag, &tag_len)==GF_OK) fprintf(stderr, "\tCreated: %s\n", tag);
		if (gf_isom_apple_get_tag(file, GF_ISOM_ITUNE_DISK, &tag, &tag_len)==GF_OK) fprintf(stderr, "\tDisk: %d / %d\n", tag[3], tag[5]);
		if (gf_isom_apple_get_tag(file, GF_ISOM_ITUNE_TOOL, &tag, &tag_len)==GF_OK) fprintf(stderr, "\tEncoder Software: %s\n", tag);
		if (gf_isom_apple_get_tag(file, GF_ISOM_ITUNE_ENCODER, &tag, &tag_len)==GF_OK) fprintf(stderr, "\tEncoded by: %s\n", tag);
		if (gf_isom_apple_get_tag(file, GF_ISOM_ITUNE_TEMPO, &tag, &tag_len)==GF_OK) {
			if (tag[0]) {
				fprintf(stderr, "\tTempo (BPM): %s\n", tag);
			} else {
				fprintf(stderr, "\tTempo (BPM): %d\n", tag[1]);
			}
		}
		if (gf_isom_apple_get_tag(file, GF_ISOM_ITUNE_TRACKNUMBER, &tag, &tag_len)==GF_OK) {
			if (tag[0]) {
				fprintf(stderr, "\tTrackNumber: %s\n", tag);
			} else {
				fprintf(stderr, "\tTrackNumber: %d / %d\n", (0xff00 & (tag[2]<<8)) | (0xff & tag[3]), (0xff00 & (tag[4]<<8)) | (0xff & tag[5]));
			}
		}
		if (gf_isom_apple_get_tag(file, GF_ISOM_ITUNE_TRACK, &tag, &tag_len)==GF_OK) fprintf(stderr, "\tTrack: %s\n", tag);
		if (gf_isom_apple_get_tag(file, GF_ISOM_ITUNE_GROUP, &tag, &tag_len)==GF_OK) fprintf(stderr, "\tGroup: %s\n", tag);

		if (gf_isom_apple_get_tag(file, GF_ISOM_ITUNE_COVER_ART, &tag, &tag_len)==GF_OK) {
			if (tag_len>>31) fprintf(stderr, "\tCover Art: PNG File\n");
			else fprintf(stderr, "\tCover Art: JPEG File\n");
		}
	}

	print_udta(file, 0);
	fprintf(stderr, "\n");
	for (i=0; i<gf_isom_get_track_count(file); i++) {
		DumpTrackInfo(file, i+1, 0, GF_TRUE);