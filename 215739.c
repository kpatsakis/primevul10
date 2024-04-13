GF_Err gf_isom_add_sample_group_info(GF_ISOFile *movie, u32 track, u32 grouping_type, void *data, u32 data_size, Bool is_default, u32 *sampleGroupDescriptionIndex)
{
	GF_Err e;
	GF_TrackBox *trak;
	GF_DefaultSampleGroupDescriptionEntry *entry=NULL;
	GF_SampleGroupDescriptionBox *sgdesc = NULL;

	if (sampleGroupDescriptionIndex) *sampleGroupDescriptionIndex = 0;
	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(movie, track);
	if (!trak) return GF_BAD_PARAM;


	sgdesc = get_sgdp(trak->Media->information->sampleTable, NULL, grouping_type, NULL);
	if (!sgdesc) return GF_OUT_OF_MEM;

	if (grouping_type==GF_ISOM_SAMPLE_GROUP_OINF) {
		GF_OperatingPointsInformation *ptr = gf_isom_oinf_new_entry();
		GF_BitStream *bs=gf_bs_new(data, data_size, GF_BITSTREAM_READ);
		e = gf_isom_oinf_read_entry(ptr, bs);
		gf_bs_del(bs);
		if (e) {
			gf_isom_oinf_del_entry(ptr);
			return e;
		}
		e = gf_list_add(sgdesc->group_descriptions, ptr);
		if (e) return e;
		entry = (GF_DefaultSampleGroupDescriptionEntry *) ptr;
	} else if (grouping_type==GF_ISOM_SAMPLE_GROUP_LINF) {
		GF_LHVCLayerInformation *ptr = gf_isom_linf_new_entry();
		GF_BitStream *bs=gf_bs_new(data, data_size, GF_BITSTREAM_READ);
		e = gf_isom_linf_read_entry(ptr, bs);
		gf_bs_del(bs);
		if (e) {
			gf_isom_linf_del_entry(ptr);
			return e;
		}
		e = gf_list_add(sgdesc->group_descriptions, ptr);
		if (e) return e;
		entry = (GF_DefaultSampleGroupDescriptionEntry *) ptr;
	} else {
		u32 i, count=gf_list_count(sgdesc->group_descriptions);
		for (i=0; i<count; i++) {
			GF_DefaultSampleGroupDescriptionEntry *ent = gf_list_get(sgdesc->group_descriptions, i);
			if ((ent->length == data_size) && !memcmp(ent->data, data, data_size)) {
				entry = ent;
				break;
			}
			entry=NULL;
		}
		if (!entry) {
			GF_SAFEALLOC(entry, GF_DefaultSampleGroupDescriptionEntry);
			if (!entry) return GF_OUT_OF_MEM;
			entry->data = (u8*)gf_malloc(sizeof(char) * data_size);
			if (!entry->data) {
				gf_free(entry);
				return GF_OUT_OF_MEM;
			}
			entry->length = data_size;
			memcpy(entry->data, data, sizeof(char) * data_size);
			e = gf_list_add(sgdesc->group_descriptions, entry);
			if (e) {
				gf_free(entry->data);
				gf_free(entry);
				return e;
			}
		}
	}


	if (is_default) {
		sgdesc->default_description_index = 1 + gf_list_find(sgdesc->group_descriptions, entry);
		sgdesc->version = 2;
	}
	if (sampleGroupDescriptionIndex) *sampleGroupDescriptionIndex = 1 + gf_list_find(sgdesc->group_descriptions, entry);

	return GF_OK;
}