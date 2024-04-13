void *sg_rap_create_entry(void *udta)
{
	GF_VisualRandomAccessEntry *entry;
	u32 *num_leading_samples = (u32 *) udta;
	assert(udta);
	GF_SAFEALLOC(entry, GF_VisualRandomAccessEntry);
	if (!entry) return NULL;
	entry->num_leading_samples = *num_leading_samples;
	entry->num_leading_samples_known = entry->num_leading_samples ? 1 : 0;
	return entry;
}