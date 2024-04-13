static __always_inline void mm_clear_owner(struct mm_struct *mm,
					   struct task_struct *p)
{
#ifdef CONFIG_MEMCG
	if (mm->owner == p)
		WRITE_ONCE(mm->owner, NULL);
#endif
}