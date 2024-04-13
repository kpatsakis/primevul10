struct mm_struct *copy_init_mm(void)
{
	return dup_mm(NULL, &init_mm);
}