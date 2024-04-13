static __inline__ unsigned cbq_hash(u32 h)
{
	h ^= h>>8;
	h ^= h>>4;
	return h&0xF;
}