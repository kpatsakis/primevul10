GF_Err gf_isom_remove_track_references(GF_ISOFile *the_file, u32 trackNumber)
{
	GF_TrackBox *trak;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	if (trak->References) {
		gf_isom_box_del_parent(&trak->child_boxes, (GF_Box *)trak->References);
		trak->References = NULL;
	}
	return GF_OK;
}