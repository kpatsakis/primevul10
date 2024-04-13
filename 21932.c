static u32 destroy_obj_hashfn(const struct tcf_proto *tp)
{
	return jhash_3words(tp->chain->index, tp->prio,
			    (__force __u32)tp->protocol, 0);
}