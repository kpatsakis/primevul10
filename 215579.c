GF_Err gf_isom_apple_set_tag(GF_ISOFile *mov, GF_ISOiTunesTag tag, const u8 *data, u32 data_len, u64 int_val, u32 int_val2)
{
	GF_Err e;
	GF_ItemListBox *ilst;
	GF_MetaBox *meta;
	GF_ListItemBox *info;
	u32 btype, i, itype;
	s32 tag_idx;
	u32 n=0, d=0;
	u8 loc_data[10];
	u32 int_flags = 0x15;
	GF_DataBox *dbox;

	e = CanAccessMovie(mov, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	tag_idx = gf_itags_find_by_itag(tag);
	if (tag_idx<0) {
		itype = GF_ITAG_STR;
	} else {
		itype = gf_itags_get_type(tag_idx);
	}
	meta = (GF_MetaBox *) gf_isom_create_meta_extensions(mov, GF_FALSE);
	if (!meta) return GF_BAD_PARAM;

	ilst = gf_ismo_locate_box(meta->child_boxes, GF_ISOM_BOX_TYPE_ILST, NULL);
	if (!ilst) {
		ilst = (GF_ItemListBox *) gf_isom_box_new_parent(&meta->child_boxes, GF_ISOM_BOX_TYPE_ILST);
	}

	if (tag==GF_ISOM_ITUNE_RESET) {
		gf_isom_box_del_parent(&meta->child_boxes, (GF_Box *) ilst);
		//if last, delete udta - we may still have a handler box remaining
		if ((gf_list_count(meta->child_boxes) <= 1) && (gf_list_count(mov->moov->udta->recordList)==1)) {
			gf_isom_box_del_parent(&mov->moov->child_boxes, (GF_Box *) mov->moov->udta);
			mov->moov->udta = NULL;
		}
		return GF_OK;
	}

	if (tag==GF_ISOM_ITUNE_GENRE) {
		if (!int_val && data) {
			int_val = gf_id3_get_genre_tag(data);
			if (int_val) {
				data = NULL;
				data_len = 0;
				itype = GF_ITAG_INT16;
				int_flags = 0;
			}
		}
		btype = data ? GF_ISOM_ITUNE_GENRE_USER : GF_ISOM_ITUNE_GENRE;
	} else {
		btype = tag;
	}
	/*remove tag*/
	i = 0;
	while ((info = (GF_ListItemBox*)gf_list_enum(ilst->child_boxes, &i))) {
		if (info->type==btype) {
			gf_isom_box_del_parent(&ilst->child_boxes, (GF_Box *) info);
			info = NULL;
			break;
		}
		if (info->type==GF_ISOM_BOX_TYPE_UNKNOWN) {
			GF_UnknownBox *u = (GF_UnknownBox *) info;
			if (u->original_4cc==btype) {
				gf_isom_box_del_parent(&ilst->child_boxes, (GF_Box *) info);
				info = NULL;
				break;
			}
		}
	}

	if (!data && data_len) {
		if (!gf_list_count(ilst->child_boxes) )
			gf_isom_box_del_parent(&meta->child_boxes, (GF_Box *) ilst);
		return GF_OK;
	}

	info = (GF_ListItemBox *)gf_isom_box_new(btype);
	if (info == NULL) return GF_OUT_OF_MEM;

	dbox = (GF_DataBox *)gf_isom_box_new_parent(&info->child_boxes, GF_ISOM_BOX_TYPE_DATA);
	if (!dbox) {
		gf_isom_box_del((GF_Box *)info);
		return GF_OUT_OF_MEM;
	}
	if (info->type!=GF_ISOM_BOX_TYPE_UNKNOWN) {
		info->data = dbox;
	}

	switch (itype) {
	case GF_ITAG_FRAC6:
	case GF_ITAG_FRAC8:
		if (data && data_len) {
			if (sscanf(data, "%u/%u", &n, &d) != 2) {
				n = d = 0;
				if (sscanf(data, "%u", &n) != 1)
					n = 0;
			}
		} else {
			n = (u32) int_val;
			d = int_val2;
		}
		if (n) {
			memset(loc_data, 0, sizeof(char) * 8);
			data_len = (itype == GF_ITAG_FRAC6) ? 6 : 8;
			loc_data[3] = n;
			loc_data[2] = n >> 8;
			loc_data[5] = d;
			loc_data[4] = d >> 8;
			data = loc_data;
		} else {
			data = NULL;
		}
		dbox->flags = 0x15;
		break;
	case GF_ITAG_BOOL:
		loc_data[0] = 0;
		if (data && data_len) {
			if ( !strcmp(data, "yes") || !strcmp(data, "1") || !strcmp(data, "true"))
				loc_data[0] = 1;
		} else {
			loc_data[0] = int_val ? 1 : 0;
		}
		data = loc_data;
		data_len = 0;
		dbox->flags = int_flags;
		break;
	case GF_ITAG_INT16:
		loc_data[0] = 0;
		if (data && data_len) int_val = atoi(data);
		loc_data[1] = (u8) int_val;
		loc_data[0] = (u8) (int_val>>8);
		data = loc_data;
		data_len = 2;
		dbox->flags = int_flags;
		break;
	case GF_ITAG_INT32:
		loc_data[0] = 0;
		if (data && data_len) int_val = atoi(data);
		loc_data[3] = (u8) int_val;
		loc_data[2] = (u8) (int_val>>8);
		loc_data[1] = (u8) (int_val>>16);
		loc_data[0] = (u8) (int_val>>24);
		data = loc_data;
		data_len = 4;
		dbox->flags = int_flags;
		break;
	case GF_ITAG_INT64:
		loc_data[0] = 0;
		if (data && data_len) sscanf(data, LLU, &int_val);
		loc_data[7] = (u8) int_val;
		loc_data[6] = (u8) (int_val>>8);
		loc_data[5] = (u8) (int_val>>16);
		loc_data[4] = (u8) (int_val>>24);
		loc_data[3] = (u8) (int_val>>32);
		loc_data[2] = (u8) (int_val>>40);
		loc_data[1] = (u8) (int_val>>48);
		loc_data[0] = (u8) (int_val>>56);
		data = loc_data;
		data_len = 4;
		dbox->flags = int_flags;
		break;
	default:
		dbox->flags = 1;
		break;
	}

	if (!data) return GF_BAD_PARAM;


	if (tag==GF_ISOM_ITUNE_COVER_ART) {
		info->data->flags = 0;
		/*check for PNG sig*/
		if ((data_len>4) && (data[0] == 0x89) && (data[1] == 0x50) && (data[2] == 0x4E) && (data[3] == 0x47) ) {
			info->data->flags = 14;
		}
		else if ((data_len>4) && (data[0] == 0xFF) && (data[1] == 0xD8) && (data[2] == 0xFF) && (data[3] == 0xE0) ) {
			info->data->flags = 13;
		}
		else if ((data_len>3) && (data[0] == 'G') && (data[1] == 'I') && (data[2] == 'F') ) {
			info->data->flags = 12;
		}
	}

	dbox->dataSize = data_len;
	dbox->data = (char*)gf_malloc(sizeof(char)*data_len);
	if (!dbox->data) return GF_OUT_OF_MEM;
	memcpy(dbox->data, data, sizeof(char)*data_len);

	if (!info && !gf_list_count(ilst->child_boxes) ) {
		gf_isom_box_del_parent(&meta->child_boxes, (GF_Box *) ilst);
		return GF_OK;
	}
	if (!ilst->child_boxes) ilst->child_boxes = gf_list_new();
	
	return gf_list_add(ilst->child_boxes, info);
}