Bool sg_roll_compare_entry(void *udta, void *entry)
{
	s16 *roll_distance = (s16 *) udta;
	if (*roll_distance == ((GF_RollRecoveryEntry *)entry)->roll_distance) return GF_TRUE;
	return GF_FALSE;
}