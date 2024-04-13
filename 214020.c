static Bool parse_meta_args(MetaAction *meta, MetaActionType act_type, char *opts)
{
	Bool ret = 0;

	memset(meta, 0, sizeof(MetaAction));
	meta->act_type = act_type;
	meta->trackID = 0;
	meta->root_meta = 1;

	if (!opts) return 0;
	while (1) {
		char *next;
		char *szSlot;
		if (!opts || !opts[0]) return ret;
		if (opts[0]==':') opts += 1;

		szSlot = opts;
		next = gf_url_colon_suffix(opts);
		if (next) next[0] = 0;

		if (!strnicmp(szSlot, "tk=", 3)) {
			sscanf(szSlot, "tk=%u", &meta->trackID);
			meta->root_meta = 0;
			ret = 1;
		}
		else if (!strnicmp(szSlot, "id=", 3)) {
			meta->item_id = atoi(szSlot+3);
			ret = 1;
		}
		else if (!strnicmp(szSlot, "type=", 5)) {
			meta->item_type = GF_4CC(szSlot[5], szSlot[6], szSlot[7], szSlot[8]);
			ret = 1;
		}
		else if (!strnicmp(szSlot, "ref=", 4)) {
			char type[10];
			sscanf(szSlot, "ref=%9s,%u", type, &meta->ref_item_id);
			meta->ref_type = GF_4CC(type[0], type[1], type[2], type[3]);
			ret = 1;
		}
		else if (!strnicmp(szSlot, "name=", 5)) {
			meta->szName = gf_strdup(szSlot+5);
			ret = 1;
		}
		else if (!strnicmp(szSlot, "path=", 5)) {
			meta->szPath = gf_strdup(szSlot+5);
			ret = 1;
		}
		else if (!strnicmp(szSlot, "mime=", 5)) {
			meta->item_type = GF_META_ITEM_TYPE_MIME;
			meta->mime_type = gf_strdup(szSlot+5);
			ret = 1;
		}
		else if (!strnicmp(szSlot, "encoding=", 9)) {
			meta->enc_type = gf_strdup(szSlot+9);
			ret = 1;
		}
		else if (!strnicmp(szSlot, "image-size=", 11)) {
			if (!meta->image_props) {
				GF_SAFEALLOC(meta->image_props, GF_ImageItemProperties);
				if (!meta->image_props) return 0;
			}

			sscanf(szSlot+11, "%dx%d", &meta->image_props->width, &meta->image_props->height);
			ret = 1;
		}
		else if (!strnicmp(szSlot, "image-pasp=", 11)) {
			if (!meta->image_props) {
				GF_SAFEALLOC(meta->image_props, GF_ImageItemProperties);
				if (!meta->image_props) return 0;
			}
			sscanf(szSlot+11, "%dx%d", &meta->image_props->hSpacing, &meta->image_props->vSpacing);
			ret = 1;
		}
		else if (!strnicmp(szSlot, "image-rloc=", 11)) {
			if (!meta->image_props) {
				GF_SAFEALLOC(meta->image_props, GF_ImageItemProperties);
				if (!meta->image_props) return 0;
			}
			sscanf(szSlot+11, "%dx%d", &meta->image_props->hOffset, &meta->image_props->vOffset);
			ret = 1;
		}
		else if (!strnicmp(szSlot, "rotation=", 9)) {
			if (!meta->image_props) {
				GF_SAFEALLOC(meta->image_props, GF_ImageItemProperties);
				if (!meta->image_props) return 0;
			}
			meta->image_props->angle = atoi(szSlot+9);
			ret = 1;
		}
		else if (!stricmp(szSlot, "hidden")) {
			if (!meta->image_props) {
				GF_SAFEALLOC(meta->image_props, GF_ImageItemProperties);
				if (!meta->image_props) return 0;
			}
			meta->image_props->hidden = GF_TRUE;
			ret = 1;
		}
		else if (!stricmp(szSlot, "alpha")) {
			if (!meta->image_props) {
				GF_SAFEALLOC(meta->image_props, GF_ImageItemProperties);
				if (!meta->image_props) return 0;
			}
			meta->image_props->alpha = GF_TRUE;
			ret = 1;
		}
		else if (!strnicmp(szSlot, "time=", 5)) {
			if (!meta->image_props) {
				GF_SAFEALLOC(meta->image_props, GF_ImageItemProperties);
				if (!meta->image_props) return 0;
			}
			meta->image_props->time = atof(szSlot+5);
			ret = 1;
		}
		else if (!stricmp(szSlot, "split_tiles")) {
			if (!meta->image_props) {
				GF_SAFEALLOC(meta->image_props, GF_ImageItemProperties);
				if (!meta->image_props) return 0;
			}
			meta->image_props->tile_mode = TILE_ITEM_ALL_BASE;
			ret = 1;
		}
		else if (!stricmp(szSlot, "dref")) {
			meta->use_dref = 1;
			ret = 1;
		}
		else if (!stricmp(szSlot, "primary")) {
			meta->primary = 1;
			ret = 1;
		}
		else if (!stricmp(szSlot, "binary")) {
			if (meta->act_type==META_ACTION_SET_XML) meta->act_type=META_ACTION_SET_BINARY_XML;
			ret = 1;
		}
		else if (!strnicmp(szSlot, "icc_path=", 9)) {
			if (!meta->image_props) {
				GF_SAFEALLOC(meta->image_props, GF_ImageItemProperties);
				if (!meta->image_props) return 0;
			}
			strcpy(meta->image_props->iccPath, szSlot+9);
			ret = 1;
		}
		else if (!strchr(szSlot, '=')) {
			switch (meta->act_type) {
			case META_ACTION_SET_TYPE:
				if (!stricmp(szSlot, "null") || !stricmp(szSlot, "0")) meta->meta_4cc = 0;
				else meta->meta_4cc = GF_4CC(szSlot[0], szSlot[1], szSlot[2], szSlot[3]);
				ret = 1;
				break;
			case META_ACTION_ADD_ITEM:
			case META_ACTION_ADD_IMAGE_ITEM:
			case META_ACTION_SET_XML:
			case META_ACTION_DUMP_XML:
				if (!strncmp(szSlot, "dopt", 4) || !strncmp(szSlot, "sopt", 4) || !strncmp(szSlot, "@@", 2)) {
					if (next) next[0]=':';
					next=NULL;
				}
				//cat as -add arg
				gf_dynstrcat(&meta->szPath, szSlot, ":");
				ret = 1;
				break;
			case META_ACTION_REM_ITEM:
			case META_ACTION_SET_PRIMARY_ITEM:
			case META_ACTION_DUMP_ITEM:
				meta->item_id = atoi(szSlot);
				ret = 1;
				break;
			default:
				break;
			}
		}
		if (!next) break;
		opts += strlen(szSlot);
		next[0] = ':';
	}
	return ret;
}