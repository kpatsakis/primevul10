GF_Err gf_isom_set_y3d_info(GF_ISOFile *movie, u32 trackNumber, u32 sampleDescriptionIndex, GF_ISOM_Y3D_Info *info)
{
	GF_Err e;
	u32 proj_type;
	GF_SampleEntryBox *ent;
	GF_TrackBox *trak;

	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak || !trak->Media || !info) return GF_BAD_PARAM;

	ent = gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, sampleDescriptionIndex-1);
	if (!ent) return GF_BAD_PARAM;

	if (info->projection_type > GF_PROJ360_EQR) return GF_NOT_SUPPORTED;

	GF_Stereo3DBox *st3d = (GF_Stereo3DBox *) gf_isom_box_find_child(ent->child_boxes, GF_ISOM_BOX_TYPE_ST3D);
	if (st3d) {
		if (info->stereo_type) {
			st3d->stereo_type = info->stereo_type;
		} else {
			gf_isom_box_del_parent(&ent->child_boxes, (GF_Box *) st3d);
		}
	} else if (info->stereo_type) {
		st3d = (GF_Stereo3DBox *) gf_isom_box_new_parent(&ent->child_boxes, GF_ISOM_BOX_TYPE_ST3D);
		if (!st3d) return GF_OUT_OF_MEM;
		st3d->stereo_type = info->stereo_type;
	}


	GF_Box *sv3d = gf_isom_box_find_child(ent->child_boxes, GF_ISOM_BOX_TYPE_SV3D);
	if (sv3d && !info->projection_type) {
		gf_isom_box_del_parent(&ent->child_boxes, sv3d);
		return GF_OK;
	}

	if (!sv3d && !info->projection_type) {
		return GF_OK;
	}
	if (!sv3d) {
		sv3d = gf_isom_box_new_parent(&ent->child_boxes, GF_ISOM_BOX_TYPE_SV3D);
		if (!sv3d) return GF_OUT_OF_MEM;
	}

	//svhd mandatory
	GF_SphericalVideoInfoBox *svhd = (GF_SphericalVideoInfoBox *) gf_isom_box_find_child(sv3d->child_boxes, GF_ISOM_BOX_TYPE_SVHD);
	if (svhd) {
		if (svhd->string) gf_free(svhd->string);
	} else {
		svhd = (GF_SphericalVideoInfoBox *) gf_isom_box_new_parent(&sv3d->child_boxes, GF_ISOM_BOX_TYPE_SVHD);
		if (!svhd) return GF_OUT_OF_MEM;
	}
	svhd->string = gf_strdup(info->meta_data ? info->meta_data : "");

	//proj mandatory
	GF_Box *proj = gf_isom_box_find_child(sv3d->child_boxes, GF_ISOM_BOX_TYPE_PROJ);
	if (!proj) {
		proj = (GF_Box *) gf_isom_box_new_parent(&sv3d->child_boxes, GF_ISOM_BOX_TYPE_PROJ);
		if (!proj) return GF_OUT_OF_MEM;
	}

	GF_ProjectionHeaderBox *projh = (GF_ProjectionHeaderBox *) gf_isom_box_find_child(proj->child_boxes, GF_ISOM_BOX_TYPE_PRHD);
	//prj header mandatory
	if (!projh) {
		projh = (GF_ProjectionHeaderBox *) gf_isom_box_new_parent(&proj->child_boxes, GF_ISOM_BOX_TYPE_PRHD);
		if (!projh) return GF_OUT_OF_MEM;
	}
	projh->yaw = info->yaw;
	projh->pitch = info->pitch;
	projh->roll = info->roll;

	proj_type = (info->projection_type==GF_PROJ360_CUBE_MAP) ? GF_ISOM_BOX_TYPE_CBMP : GF_ISOM_BOX_TYPE_EQUI;

	GF_ProjectionTypeBox *projt = (GF_ProjectionTypeBox *) gf_isom_box_find_child(proj->child_boxes, proj_type);
	if (!projt) {
		projt = (GF_ProjectionTypeBox *) gf_isom_box_new_parent(&proj->child_boxes, proj_type);
		if (!projt) return GF_OUT_OF_MEM;
	}
	if (info->projection_type==GF_PROJ360_CUBE_MAP) {
		projt->layout = info->layout;
		projt->padding = info->padding;
	} else {
		projt->bounds_top = info->top;
		projt->bounds_bottom = info->bottom;
		projt->bounds_left = info->left;
		projt->bounds_right = info->right;
	}

	//remove other ones
	GF_Box *b = gf_isom_box_new_parent(&proj->child_boxes, GF_ISOM_BOX_TYPE_MSHP);
	if (b) gf_isom_box_del_parent(&proj->child_boxes, b);
	if (info->projection_type==GF_PROJ360_CUBE_MAP) {
		b = gf_isom_box_new_parent(&proj->child_boxes, GF_ISOM_BOX_TYPE_EQUI);
		if (b) gf_isom_box_del_parent(&proj->child_boxes, b);
	} else {
		b = gf_isom_box_new_parent(&proj->child_boxes, GF_ISOM_BOX_TYPE_EQUI);
		if (b) gf_isom_box_del_parent(&proj->child_boxes, b);

	}
	return GF_OK;
}