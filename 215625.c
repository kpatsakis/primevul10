GF_Err gf_isom_apply_box_patch(GF_ISOFile *file, GF_ISOTrackID globalTrackID, const char *box_patch_filename, Bool for_fragments)
{
	GF_Err e;
	GF_DOMParser *dom;
	u32 i;
	GF_XMLNode *root;
	u8 *box_data=NULL;
	u32 box_data_size;
	if (!file || !box_patch_filename) return GF_BAD_PARAM;
	dom = gf_xml_dom_new();
	if (strstr(box_patch_filename, "<?xml")) {
		e = gf_xml_dom_parse_string(dom, (char *) box_patch_filename);
	} else {
		e = gf_xml_dom_parse(dom, box_patch_filename, NULL, NULL);
	}
	if (e) goto err_exit;

	root = gf_xml_dom_get_root(dom);
	if (!root || strcmp(root->name, "GPACBOXES")) {
		e = GF_NON_COMPLIANT_BITSTREAM;
		goto err_exit;
	}

	//compute size of each child boxes to freeze the order
	if (for_fragments) {
		u32 count = file->moof ? gf_list_count(file->moof->child_boxes) : 0;
		for (i=0; i<count; i++) {
			GF_Box *box = gf_list_get(file->moof->child_boxes, i);
			if (!(box->internal_flags & GF_ISOM_ORDER_FREEZE)) {
				gf_isom_box_size(box);
				gf_isom_box_freeze_order(box);
			}
		}
	} else {
		for (i=0; i<gf_list_count(file->TopBoxes); i++) {
			GF_Box *box = gf_list_get(file->TopBoxes, i);
			if (!(box->internal_flags & GF_ISOM_ORDER_FREEZE)) {
				gf_isom_box_size(box);
				gf_isom_box_freeze_order(box);
			}
		}
	}

	for (i=0; i<gf_list_count(root->content); i++) {
		u32 j;
		u32 path_len;
		Bool essential_prop=GF_FALSE;
		u32 trackID=globalTrackID;
		u32 item_id=trackID;
		Bool is_frag_box;
		char *box_path=NULL;
		GF_Box *parent_box = NULL;
		GF_XMLNode *box_edit = gf_list_get(root->content, i);
		if (!box_edit->name || strcmp(box_edit->name, "Box")) continue;

		for (j=0; j<gf_list_count(box_edit->attributes);j++) {
			GF_XMLAttribute *att = gf_list_get(box_edit->attributes, j);
			if (!strcmp(att->name, "path")) box_path = att->value;
			else if (!strcmp(att->name, "essential")) {
				if (!strcmp(att->value, "yes") || !strcmp(att->value, "true") || !strcmp(att->value, "1")) {
					essential_prop=GF_TRUE;
				}
			}
			else if (!strcmp(att->name, "itemID"))
				item_id = atoi(att->value);
			else if (!globalTrackID && !strcmp(att->name, "trackID"))
				trackID = atoi(att->value);
		}

		if (!box_path) continue;
		path_len = (u32) strlen(box_path);

		is_frag_box = !strncmp(box_path, "traf", 4) ? GF_TRUE : GF_FALSE;

		if (for_fragments && !is_frag_box) continue;
		else if (!for_fragments && is_frag_box) continue;

		gf_xml_parse_bit_sequence(box_edit, box_patch_filename, &box_data, &box_data_size);
		if (box_data_size && (box_data_size<4) ) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[ISOBMFF] Wrong BS specification for box, shall either be empty or at least 4 bytes for box type\n"));
			e = GF_NON_COMPLIANT_BITSTREAM;
			goto err_exit;
		}

		while (box_path && (path_len>=4)) {
			u32 parent_list_box_type;
			GF_List **parent_list;
			u32 box_type = GF_4CC(box_path[0],box_path[1],box_path[2],box_path[3]);
			GF_Box *box=NULL;
			GF_BitStream *bs;
			s32 insert_pos = -1;
			box_path+=4;
			path_len-=4;

			if (!parent_box) {
				box=gf_isom_box_find_child(file->TopBoxes, box_type);
				if (!box) {
					if (box_type==GF_ISOM_BOX_TYPE_TRAK) {
						if (trackID) {
							box = (GF_Box *) gf_isom_get_track_from_file(file, gf_isom_get_track_by_id(file, trackID) );
						}
						if (!box && gf_list_count(file->moov->trackList)==1) {
							box = gf_list_get(file->moov->trackList, 0);
						}
					}
					else if (box_type==GF_ISOM_BOX_TYPE_TRAF) {
						if (trackID) {
							box = (GF_Box *) gf_isom_get_traf(file, trackID);
						}
						if (!box && file->moof && gf_list_count(file->moof->TrackList)==1) {
							box = gf_list_get(file->moof->TrackList, 0);
						}
					}
				}
				if (!box) {
					GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[ISOBMFF] Cannot locate box type %s at root or as track\n", gf_4cc_to_str(box_type) ));
					e = GF_BAD_PARAM;
					goto err_exit;
				}
			} else {
				box = gf_isom_box_find_child(parent_box->child_boxes, box_type);
				if (!box) {
					GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[ISOBMFF] Cannot locate box type %s at child of %s\n", gf_4cc_to_str(box_type), gf_4cc_to_str(parent_box->type)));
					e = GF_BAD_PARAM;
					goto err_exit;
				}
			}
			// '.' is child access
			if (path_len && (box_path[0]=='.')) {
				box_path += 1;
				path_len-=1;
				parent_box = box;
				continue;
			}

			if (parent_box && !parent_box->child_boxes) parent_box->child_boxes = gf_list_new();
			parent_list = parent_box ? &parent_box->child_boxes : &file->TopBoxes;
			parent_list_box_type = parent_box ? parent_box->type : 0;

			// '+' is append after, '-' is insert before
			if (path_len && ((box_path[0]=='-') || (box_path[0]=='+')) ) {
				s32 idx = gf_list_find(*parent_list, box);
				assert(idx>=0);
				if (box_path[0]=='+') insert_pos = idx+1;
				else insert_pos = idx;
			}
			else if (path_len) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[ISOBMFF] Invalid path %s, expecting either '-', '+' or '.' as separators\n", box_path));
				e = GF_NON_COMPLIANT_BITSTREAM;
				goto err_exit;
			}

			if (!box_data) {
				if (insert_pos>=0) {
					GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[ISOBMFF] Invalid path %s for box removal, ignoring position\n", box_path));
				}
				switch (box->type) {
				case GF_ISOM_BOX_TYPE_MOOV:
					file->moov = NULL;
					break;
				case GF_ISOM_BOX_TYPE_MDAT:
					file->mdat = NULL;
					break;
				case GF_ISOM_BOX_TYPE_PDIN:
					file->pdin = NULL;
					break;
				case GF_ISOM_BOX_TYPE_FTYP:
					file->brand = NULL;
					break;
				case GF_ISOM_BOX_TYPE_META:
					if ((GF_Box *) file->meta == box) file->meta = NULL;
					break;
				}
				if (parent_box) {
					gf_isom_box_remove_from_parent(parent_box, box);
				}
				gf_isom_box_del_parent(parent_list, box);
			} else {
				u32 size;

				bs = gf_bs_new(box_data, box_data_size, GF_BITSTREAM_READ);
				size = gf_bs_read_u32(bs);
				if (size != box_data_size) {
					GF_UnknownBox *new_box = (GF_UnknownBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_UNKNOWN);
					new_box->original_4cc = size;
					new_box->dataSize = (u32) gf_bs_available(bs);
					new_box->data = gf_malloc(sizeof(u8)*new_box->dataSize);
					gf_bs_read_data(bs, new_box->data, new_box->dataSize);
					if (insert_pos<0) {
						gf_list_add(box->child_boxes, new_box);
						insert_pos = gf_list_find(box->child_boxes, new_box);
					} else {
						gf_list_insert(*parent_list, new_box, insert_pos);
					}

					if (parent_box && (parent_box->type==GF_ISOM_BOX_TYPE_IPRP)) {
						GF_ItemPropertyAssociationBox *ipma = (GF_ItemPropertyAssociationBox *) gf_isom_box_find_child(parent_box->child_boxes, GF_ISOM_BOX_TYPE_IPMA);
						if (!item_id) {
							GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[ISOBMFF] Inserting box in ipco without itemID, no association added\n"));
						} else if (ipma) {
							u32 nb_asso, k;
							GF_ItemPropertyAssociationEntry *entry = NULL;
							nb_asso = gf_list_count(ipma->entries);
							for (k=0; k<nb_asso;k++) {
								entry = gf_list_get(ipma->entries, k);
								if (entry->item_id==item_id) break;
								entry = NULL;
							}
							if (!entry) {
								GF_SAFEALLOC(entry, GF_ItemPropertyAssociationEntry);
								if (!entry) return GF_OUT_OF_MEM;
								gf_list_add(ipma->entries, entry);
								entry->item_id = item_id;
							}
							entry->associations = gf_realloc(entry->associations, sizeof(GF_ItemPropertyAssociationSlot) * (entry->nb_associations+1));
							entry->associations[entry->nb_associations].essential = essential_prop;
							entry->associations[entry->nb_associations].index = 1+insert_pos;
							entry->nb_associations++;
						}
					}
				} else {
					u32 box_idx = 0;

					gf_bs_seek(bs, 0);
					while (gf_bs_available(bs)) {
						GF_Box *new_box;
						e = gf_isom_box_parse_ex(&new_box, bs, (insert_pos<0) ? box->type : parent_list_box_type, parent_box ? GF_FALSE : GF_TRUE);
						if (e) {
							GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[ISOBMFF] failed to parse box\n", box_path));
							gf_bs_del(bs);
							goto err_exit;
						}
						if (insert_pos<0) {
							gf_list_add(box->child_boxes, new_box);
						} else {
							gf_list_insert(*parent_list, new_box, insert_pos+box_idx);
							box_idx++;
						}
					}
				}
				gf_bs_del(bs);

			}
			gf_free(box_data);
			box_data = NULL;
			box_path = NULL;
		}
	}

err_exit:

	gf_xml_dom_del(dom);
	if (box_data) gf_free(box_data);
	return e;
}