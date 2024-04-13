static __inline__ unsigned hash_dst(u32 *dst, u8 protocol, u8 tunnelid)
{
	unsigned h = dst[RSVP_DST_LEN-1];
	h ^= h>>16;
	h ^= h>>8;
	return (h ^ protocol ^ tunnelid) & 0xFF;
}