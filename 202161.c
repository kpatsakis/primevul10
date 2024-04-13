static __inline__ unsigned hash_src(u32 *src)
{
	unsigned h = src[RSVP_DST_LEN-1];
	h ^= h>>16;
	h ^= h>>8;
	h ^= h>>4;
	return h & 0xF;
}