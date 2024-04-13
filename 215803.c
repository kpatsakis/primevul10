GF_Err stbl_SetSyncShadow(GF_ShadowSyncBox *stsh, u32 sampleNumber, u32 syncSample)
{
	u32 i, count;
	GF_StshEntry *ent;

	count = gf_list_count(stsh->entries);
	for (i=0; i<count; i++) {
		ent = (GF_StshEntry*)gf_list_get(stsh->entries, i);
		if (ent->shadowedSampleNumber == sampleNumber) {
			ent->syncSampleNumber = syncSample;
			return GF_OK;
		}
		if (ent->shadowedSampleNumber > sampleNumber) break;
	}
	//we need a new one...
	ent = (GF_StshEntry*)gf_malloc(sizeof(GF_StshEntry));
	if (!ent) return GF_OUT_OF_MEM;
	ent->shadowedSampleNumber = sampleNumber;
	ent->syncSampleNumber = syncSample;
	//insert or append ?
	if (i == gf_list_count(stsh->entries)) {
		//don't update the cache ...
		return gf_list_add(stsh->entries, ent);
	} else {
		//update the cache
		stsh->r_LastEntryIndex = i;
		stsh->r_LastFoundSample = sampleNumber;
		return gf_list_insert(stsh->entries, ent, i);
	}
}