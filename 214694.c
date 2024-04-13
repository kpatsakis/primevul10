static void dump_qt_prores(GF_ISOFile *file, GF_ISOTrackID trackID, FILE *dump, Bool dump_crc);

void dump_isom_nal(GF_ISOFile *file, GF_ISOTrackID trackID, char *inName, Bool is_final_name, u32 dump_flags)
{
	Bool is_av1 = GF_FALSE;
	Bool is_prores = GF_FALSE;

	FILE *dump;
	if (inName) {
		GF_ESD* esd;
		char szBuf[GF_MAX_PATH];

		strcpy(szBuf, inName);

		u32 track = gf_isom_get_track_by_id(file, trackID);
		esd = gf_isom_get_esd(file, track, 1);

		if (!esd || !esd->decoderConfig) {
			switch (gf_isom_get_media_subtype(file, track, 1)) {
			case GF_ISOM_SUBTYPE_AV01:
				is_av1 = GF_TRUE;
				break;
			case GF_QT_SUBTYPE_APCH:
			case GF_QT_SUBTYPE_APCO:
			case GF_QT_SUBTYPE_APCN:
			case GF_QT_SUBTYPE_APCS:
			case GF_QT_SUBTYPE_AP4X:
			case GF_QT_SUBTYPE_AP4H:
				is_prores = GF_TRUE;
				break;
			}
		}
		else if (esd->decoderConfig->objectTypeIndication == GF_CODECID_AV1) {
			is_av1 = GF_TRUE;
		}
		if (esd) gf_odf_desc_del((GF_Descriptor*)esd);

		if (!is_final_name) sprintf(szBuf, "%s_%d_%s.xml", inName, trackID, is_av1 ? "obu" : "nalu");
		dump = gf_fopen(szBuf, "wt");
		if (!dump) {
			fprintf(stderr, "Failed to open %s for dumping\n", szBuf);
			return;
		}
	} else {
		dump = stdout;
	}

	if (is_av1)
		dump_isom_obu(file, trackID, dump, dump_flags);
	else if (is_prores)
		dump_qt_prores(file, trackID, dump, dump_flags);
	else
		dump_isom_nal_ex(file, trackID, dump, dump_flags);
