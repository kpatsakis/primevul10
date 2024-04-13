static u64 gen_hi_min(struct tnum var)
{
	return var.value & ~0xffffffffULL;
}