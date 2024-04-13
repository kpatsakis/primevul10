

static void DumpMetaItem(GF_ISOFile *file, Bool root_meta, u32 tk_num, char *name)
{
	u32 i, count, brand, primary_id;
	brand = gf_isom_get_meta_type(file, root_meta, tk_num);
	if (!brand) return;

	count = gf_isom_get_meta_item_count(file, root_meta, tk_num);
	primary_id = gf_isom_get_meta_primary_item_id(file, root_meta, tk_num);
	fprintf(stderr, "%s type: \"%s\" - %d resource item(s)\n", name, gf_4cc_to_str(brand), (count+(primary_id>0)));
	switch (gf_isom_has_meta_xml(file, root_meta, tk_num)) {
	case 1:
		fprintf(stderr, "Meta has XML resource\n");
		break;
	case 2:
		fprintf(stderr, "Meta has BinaryXML resource\n");
		break;
	}
	if (primary_id) {
		fprintf(stderr, "Primary Item - ID %d\n", primary_id);
	}
	for (i=0; i<count; i++) {
		const char *it_name, *mime, *enc, *url, *urn;
		Bool self_ref;
		u32 ID;
		u32 it_type;
		gf_isom_get_meta_item_info(file, root_meta, tk_num, i+1, &ID, &it_type, NULL, &self_ref, &it_name, &mime, &enc, &url, &urn);
		fprintf(stderr, "Item #%d - ID %d - type %s ", i+1, ID, gf_4cc_to_str(it_type));
		if (self_ref) fprintf(stderr, " - Self-Reference");
		else if (it_name) fprintf(stderr, " - Name: %s", it_name);
		if (mime) fprintf(stderr, " - MimeType: %s", mime);
		if (enc) fprintf(stderr, " - ContentEncoding: %s", enc);
		fprintf(stderr, "\n");
		if (url) fprintf(stderr, "URL: %s\n", url);
		if (urn) fprintf(stderr, "URN: %s\n", urn);