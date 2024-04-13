static u64 gen_hi_max(struct tnum var)
{
	return (var.value | var.mask) & ~0xffffffffULL;
}