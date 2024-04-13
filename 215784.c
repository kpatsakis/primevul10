GF_Err stbl_AddShadow(GF_ShadowSyncBox *stsh, u32 sampleNumber, u32 shadowNumber)
{
	GF_StshEntry *ent;
	u32 i, count;
	count = gf_list_count(stsh->entries);
	for (i=0; i<count; i++) {
		ent = (GF_StshEntry*)gf_list_get(stsh->entries, i);
		if (ent->shadowedSampleNumber == shadowNumber) {
			ent->syncSampleNumber = sampleNumber;
			return GF_OK;
		} else if (ent->shadowedSampleNumber > shadowNumber) break;
	}
	ent = (GF_StshEntry*)gf_malloc(sizeof(GF_StshEntry));
	if (!ent) return GF_OUT_OF_MEM;
	ent->shadowedSampleNumber = shadowNumber;
	ent->syncSampleNumber = sampleNumber;
	if (i == gf_list_count(stsh->entries)) {
		return gf_list_add(stsh->entries, ent);
	} else {
		return gf_list_insert(stsh->entries, ent, i ? i-1 : 0);
	}
}