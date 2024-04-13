Bool sg_rap_compare_entry(void *udta, void *entry)
{
	u32 *num_leading_samples = (u32 *) udta;
	if (*num_leading_samples == ((GF_VisualRandomAccessEntry *)entry)->num_leading_samples) return GF_TRUE;
	return GF_FALSE;
}