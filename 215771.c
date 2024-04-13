GF_Err stbl_AppendTrafMap(GF_SampleTableBox *stbl, Bool is_seg_start, u64 seg_start_offset, u64 frag_start_offset, u8 *moof_template, u32 moof_template_size, u64 sidx_start, u64 sidx_end)
{
	GF_TrafToSampleMap *tmap;
	GF_TrafMapEntry *tmap_ent;
	if (!stbl->traf_map) {
		//nope, create one
		GF_SAFEALLOC(stbl->traf_map, GF_TrafToSampleMap);
		if (!stbl->traf_map) return GF_OUT_OF_MEM;
	}
	tmap = stbl->traf_map;
	if (tmap->nb_entries >= stbl->SampleSize->sampleCount) {
		u32 i;
		for (i=0; i<tmap->nb_entries; i++) {
			if (tmap->frag_starts[i].moof_template)
				gf_free(tmap->frag_starts[i].moof_template);
		}
		memset(tmap->frag_starts, 0, sizeof(GF_TrafMapEntry)*tmap->nb_alloc);
		tmap->nb_entries = 0;
	}

	if (tmap->nb_entries + 1 > tmap->nb_alloc) {
		tmap->nb_alloc++;
		tmap->frag_starts = gf_realloc(tmap->frag_starts, sizeof(GF_TrafMapEntry) * tmap->nb_alloc);
		if (!tmap->frag_starts) return GF_OUT_OF_MEM;
	}
	tmap_ent = &tmap->frag_starts[tmap->nb_entries];
	tmap->nb_entries += 1;

	memset(tmap_ent, 0, sizeof(GF_TrafMapEntry));
	tmap_ent->sample_num = stbl->SampleSize->sampleCount;
	tmap_ent->moof_template = moof_template;
	tmap_ent->moof_template_size = moof_template_size;
	tmap_ent->moof_start = frag_start_offset;
	tmap_ent->sidx_start = sidx_start;
	tmap_ent->sidx_end = sidx_end;
	if (is_seg_start)
		tmap_ent->seg_start_plus_one = 1 + seg_start_offset;

	return GF_OK;
}