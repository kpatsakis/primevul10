}

static void DumpMetaItem(GF_ISOFile *file, Bool root_meta, u32 tk_num, char *name)
{
	char szInd[2];
	u32 i, count, primary_id;
	u32 meta_type = gf_isom_get_meta_type(file, root_meta, tk_num);
	if (name[0]=='\t') {
		szInd[0] = '\t';
		szInd[1] = 0;
	} else {
		szInd[0] = 0;
	}

	count = gf_isom_get_meta_item_count(file, root_meta, tk_num);
	primary_id = gf_isom_get_meta_primary_item_id(file, root_meta, tk_num);
	fprintf(stderr, "%s type: \"%s\" - %d resource item(s)\n", name, meta_type ? gf_4cc_to_str(meta_type) : "undefined", (count+(primary_id>0)));
	switch (gf_isom_has_meta_xml(file, root_meta, tk_num)) {
	case 1:
		fprintf(stderr, "%sMeta has XML resource\n", szInd);
		break;
	case 2:
		fprintf(stderr, "%sMeta has BinaryXML resource\n", szInd);
		break;
	}
	if (primary_id) {
		fprintf(stderr, "%sPrimary Item - ID %d\n", szInd, primary_id);
	}
	for (i=0; i<count; i++) {
		const char *it_name, *mime, *enc, *url, *urn;
		Bool self_ref;
		u32 ID;
		u32 it_type, cenc_scheme, cenc_version;
		GF_Err e = gf_isom_get_meta_item_info(file, root_meta, tk_num, i+1, &ID, &it_type, &cenc_scheme, &cenc_version, &self_ref, &it_name, &mime, &enc, &url, &urn);
		if (e) {
			fprintf(stderr, "%sItem #%d fetch info error: %s\n", szInd, i+1, gf_error_to_string(e) );
			continue;
		}
		fprintf(stderr, "%sItem #%d: ID %d type %s", szInd, i+1, ID, gf_4cc_to_str(it_type));
		if (self_ref) fprintf(stderr, " Self-Reference");
		else if (it_name && it_name[0]) fprintf(stderr, " Name \"%s\"", it_name);
		if (mime) fprintf(stderr, " MIME: \"%s\"", mime);
		if (enc) fprintf(stderr, " ContentEncoding: \"%s\"", enc);

		if (meta_type == GF_META_ITEM_TYPE_PICT) {
			GF_ImageItemProperties img_props;
			e = gf_isom_get_meta_image_props(file, root_meta, tk_num, ID, &img_props);
			if (e) {
				fprintf(stderr, " invalid image properties !");
			} else {
				u32 j;
				Bool chan_diff = 0;
				if (img_props.width && img_props.height) {
					fprintf(stderr, " size %ux%u", img_props.width, img_props.height);
				}
				if (img_props.hSpacing && img_props.vSpacing) {
					fprintf(stderr, " SAR %u/%u", img_props.hSpacing, img_props.vSpacing);
				}
				if (img_props.num_channels) {
					fprintf(stderr, " %d channel%s (", img_props.num_channels, (img_props.num_channels>1) ? "s" : "");
					for (j=1; j<img_props.num_channels; j++) {
						if (img_props.bits_per_channel[0] != img_props.bits_per_channel[j])
							chan_diff = 1;
					}
					if (chan_diff) {
						for (j=0; j<img_props.num_channels; j++) {
							if (j) fprintf(stderr, ",");
							fprintf(stderr, "%d", img_props.bits_per_channel[j]);
						}
					} else {
						fprintf(stderr, "%d", img_props.bits_per_channel[0]);
					}
					fprintf(stderr, " bpc)");
				}
				if (img_props.hOffset || img_props.vOffset)
					fprintf(stderr, " Offset %ux%u", img_props.hOffset, img_props.vOffset);
				if (img_props.alpha) fprintf(stderr, " Alpha");
				if (img_props.hidden) fprintf(stderr, " Hidden");
				if (img_props.angle) fprintf(stderr, " Rotate %d", img_props.angle);
				if (img_props.mirror) fprintf(stderr, " Mirror %d", img_props.mirror);
				if (img_props.clap_hden || img_props.clap_wden)
					fprintf(stderr, " Clap %d/%d,%d/%d,%d/%d,%d/%d", img_props.clap_wnum, img_props.clap_wden, img_props.clap_hnum, img_props.clap_hden, img_props.clap_honum, img_props.clap_hoden, img_props.clap_vonum, img_props.clap_voden);
			}
		}

		if (cenc_scheme) {
			Bool is_protected;
			u8 skip_byte_block, crypt_byte_block;
			const u8 *key_info;
			u32 key_info_size;
			fprintf(stderr, " - Protection scheme: %s v0x%08X", gf_4cc_to_str(cenc_scheme), cenc_version);

			gf_isom_extract_meta_item_get_cenc_info(file, root_meta, tk_num, ID, &is_protected, &skip_byte_block, &crypt_byte_block, &key_info, &key_info_size, NULL, NULL, NULL, NULL);
			if (skip_byte_block && crypt_byte_block)
				fprintf(stderr, " - Pattern %d:%d", skip_byte_block, crypt_byte_block);
			fprintf(stderr, "\n");
			dump_key_info(key_info, key_info_size, is_protected);
		}
		fprintf(stderr, "\n");
		if (url) fprintf(stderr, "%sURL: %s\n", szInd, url);
		if (urn) fprintf(stderr, "%sURN: %s\n", szInd, urn);
