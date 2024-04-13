void exec_mm_release(struct task_struct *tsk, struct mm_struct *mm)
{
	futex_exec_release(tsk);
	mm_release(tsk, mm);
}