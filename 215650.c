GF_Err gf_isom_wma_set_tag(GF_ISOFile *mov, char *name, char *value)
{
	GF_Err e;
	GF_XtraTag *tag=NULL;
	u32 count, i;
	GF_XtraBox *xtra;

	e = CanAccessMovie(mov, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	gf_isom_create_meta_extensions(mov, GF_FALSE);

	xtra = (GF_XtraBox *) gf_isom_create_meta_extensions(mov, GF_TRUE);
	if (!xtra) return GF_BAD_PARAM;

	count = gf_list_count(xtra->tags);
	for (i=0; i<count; i++) {
		tag = gf_list_get(xtra->tags, i);
		if (name && tag->name && !strcmp(tag->name, name)) {

		} else {
			tag = NULL;
			continue;
		}

		if (!value) {
			gf_list_rem(xtra->tags, i);
			gf_free(tag->name);
			if (tag->prop_value) gf_free(tag->prop_value);
			gf_free(tag);
			return GF_OK;
		}
		gf_free(tag->prop_value);
		tag->prop_value = 0;
	}
	if (!tag) {
		if (!name) return GF_OK;

		GF_SAFEALLOC(tag, GF_XtraTag);
		tag->name = gf_strdup(name);
		tag->prop_type = 0;
		tag->flags = 1;
		gf_list_add(xtra->tags, tag);
	}

	u32 len = (u32) strlen(value);
	tag->prop_value = gf_malloc(sizeof(u16) * (len+1) );
	memset(tag->prop_value, 0, sizeof(u16) * (len+1) );
	if (len) {
		u32 _len = (u32) gf_utf8_mbstowcs((u16 *) tag->prop_value, len, (const char **) &value);
		if (_len != (u32) -1) {
			tag->prop_value[2 * _len] = 0;
			tag->prop_value[2 * _len + 1] = 0;
		}
		tag->prop_size = 2 * _len + 2;
	} else {
		tag->prop_size = 2;
	}
	return GF_OK;
}