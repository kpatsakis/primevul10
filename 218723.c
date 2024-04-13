static int dup_mmap(struct mm_struct *mm, struct mm_struct *oldmm)
{
	mmap_write_lock(oldmm);
	RCU_INIT_POINTER(mm->exe_file, get_mm_exe_file(oldmm));
	mmap_write_unlock(oldmm);
	return 0;
}