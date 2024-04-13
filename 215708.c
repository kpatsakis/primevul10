void *sg_roll_create_entry(void *udta)
{
	GF_RollRecoveryEntry *entry;
	s16 *roll_distance = (s16 *) udta;
	GF_SAFEALLOC(entry, GF_RollRecoveryEntry);
	if (!entry) return NULL;
	entry->roll_distance = *roll_distance;
	return entry;
}