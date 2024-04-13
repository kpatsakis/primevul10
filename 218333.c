invalidate_csb_entries(const u32 *first, const u32 *last)
{
	clflush((void *)first);
	clflush((void *)last);
}