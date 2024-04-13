GF_Err gf_isom_update_aperture_info(GF_ISOFile *movie, u32 trackNumber, Bool remove)
{
	GF_Err e;
	GF_Box *box, *enof, *prof, *clef;
	GF_TrackBox *trak;
	GF_VisualSampleEntryBox *ventry;
	GF_PixelAspectRatioBox *pasp;
	GF_CleanApertureBox *clap;
	u32 j, hspacing, vspacing, clap_width_num, clap_width_den, clap_height_num, clap_height_den, high, low;
	Double width, height;

	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	if (remove) {
		if (trak->Aperture) {
			gf_isom_box_del(trak->Aperture);
			trak->Aperture = NULL;
		}
		return GF_OK;
	}
	enof = prof = clef = NULL;
	if (!trak->Aperture) {
		trak->Aperture = gf_isom_box_new_parent(&trak->child_boxes, GF_QT_BOX_TYPE_TAPT);
		if (!trak->Aperture) return GF_OUT_OF_MEM;
	}
	if (!trak->Aperture->child_boxes) {
		trak->Aperture->child_boxes = gf_list_new();
		if (!trak->Aperture->child_boxes)
			return GF_OUT_OF_MEM;
	}

	j=0;
	while ( (box = gf_list_enum(trak->Aperture->child_boxes, &j))) {
		switch (box->type) {
		case GF_QT_BOX_TYPE_CLEF: clef = box; break;
		case GF_QT_BOX_TYPE_PROF: prof = box; break;
		case GF_QT_BOX_TYPE_ENOF: enof = box; break;
		}
	}
	if (!clef) {
		clef = gf_isom_box_new(GF_QT_BOX_TYPE_CLEF);
		if (!clef) return GF_OUT_OF_MEM;
		gf_list_add(trak->Aperture->child_boxes, clef);
	}
	if (!enof) {
		enof = gf_isom_box_new(GF_QT_BOX_TYPE_ENOF);
		if (!enof) return GF_OUT_OF_MEM;
		gf_list_add(trak->Aperture->child_boxes, enof);
	}
	if (!prof) {
		prof = gf_isom_box_new(GF_QT_BOX_TYPE_PROF);
		if (!prof) return GF_OUT_OF_MEM;
		gf_list_add(trak->Aperture->child_boxes, prof);
	}

	ventry = (GF_VisualSampleEntryBox *)gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, 0);
	//no support for generic sample entries (eg, no MPEG4 descriptor)
	if (ventry == NULL) return GF_BAD_PARAM;
	if (!movie->keep_utc)
		trak->Media->mediaHeader->modificationTime = gf_isom_get_mp4time();

	if (ventry->internal_type != GF_ISOM_SAMPLE_ENTRY_VIDEO) return GF_BAD_PARAM;

	pasp = (GF_PixelAspectRatioBox *) gf_isom_box_find_child(ventry->child_boxes, GF_ISOM_BOX_TYPE_PASP);
	hspacing = vspacing = 0;
	if (pasp) {
		hspacing = pasp->hSpacing;
		vspacing = pasp->vSpacing;
	}
	clap_width_num = ventry->Width;
	clap_width_den = 1;
	clap_height_num = ventry->Height;
	clap_height_den = 1;
	clap = (GF_CleanApertureBox *) gf_isom_box_find_child(ventry->child_boxes, GF_ISOM_BOX_TYPE_CLAP);
	if (clap) {
		clap_width_num = clap->cleanApertureWidthN;
		clap_width_den = clap->cleanApertureWidthD;
		clap_height_num = clap->cleanApertureHeightN;
		clap_height_den = clap->cleanApertureHeightD;
	}
	//enof: encoded pixels in 16.16
	((GF_ApertureBox *)enof)->width = (ventry->Width)<<16;
	((GF_ApertureBox *)enof)->height = (ventry->Height)<<16;

	//prof: encoded pixels + pasp in 16.16
	width = (Float) (ventry->Width * hspacing);
	width /= vspacing;
	high = (u32) floor((Float)width);
	low = (u32) ( 0xFFFF * (width - (Double)high) );
	((GF_ApertureBox *)prof)->width = (high)<<16 | low;
	((GF_ApertureBox *)prof)->height = (ventry->Height)<<16;

	//clef: encoded pixels + pasp + clap in 16.16
	width = (Double) (clap_width_num * hspacing);
	width /= clap_width_den * vspacing;
	height = (Float) clap_height_num;
	height /= clap_height_den;

	high = (u32) floor((Float)width);
	low = (u32) (0xFFFF * (width - (Double)high));
	((GF_ApertureBox *)clef)->width = (high)<<16 | low;
	high = (u32) floor((Float)height);
	low = (u32) (0xFFFF * (height - (Double)high));
	((GF_ApertureBox *)clef)->height = (high)<<16 | low;


	return GF_OK;
}