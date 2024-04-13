GF_Err gf_isom_set_media_language(GF_ISOFile *movie, u32 trackNumber, char *code)
{
	GF_Err e;
	GF_TrackBox *trak;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak || !code) return GF_BAD_PARAM;
	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	// Old language-storage processing
	// if the new code is on 3 chars, we use it
	// otherwise, we find the associated 3 chars code and use it
	if (strlen(code) == 3) {
		memcpy(trak->Media->mediaHeader->packedLanguage, code, sizeof(char)*3);
	} else {
		s32 lang_idx;
		const char *code_3cc;
		lang_idx = gf_lang_find(code);
		if (lang_idx == -1) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("The given code is not a valid one: %s, using 'und' as 3-letter code\n", code));
			code_3cc = "und";
		} else {
			code_3cc = gf_lang_get_3cc(lang_idx);
		}
		memcpy(trak->Media->mediaHeader->packedLanguage, code_3cc, sizeof(char)*3);
	}

	// New language-storage processing
	// change the code in the extended language box (if any)
	// otherwise add an extended language box only if the given code is not 3 chars
	{
		u32 i, count;
		GF_ExtendedLanguageBox *elng;
		elng = NULL;
		count = gf_list_count(trak->Media->child_boxes);
		for (i = 0; i < count; i++) {
			GF_Box *box = (GF_Box *)gf_list_get(trak->Media->child_boxes, i);
			if (box->type == GF_ISOM_BOX_TYPE_ELNG) {
				elng = (GF_ExtendedLanguageBox *)box;
				break;
			}
		}
		if (!elng && (strlen(code) > 3)) {
			elng = (GF_ExtendedLanguageBox *)gf_isom_box_new_parent(&trak->Media->child_boxes, GF_ISOM_BOX_TYPE_ELNG);
			if (!elng) return GF_OUT_OF_MEM;
		}
		if (elng) {
			if (elng->extended_language) {
				gf_free(elng->extended_language);
			}
			elng->extended_language = gf_strdup(code);
		}
	}
	if (!movie->keep_utc)
		trak->Media->mediaHeader->modificationTime = gf_isom_get_mp4time();
	return GF_OK;
}