static void copy_oom_score_adj(u64 clone_flags, struct task_struct *tsk)
{
	/* Skip if kernel thread */
	if (!tsk->mm)
		return;

	/* Skip if spawning a thread or using vfork */
	if ((clone_flags & (CLONE_VM | CLONE_THREAD | CLONE_VFORK)) != CLONE_VM)
		return;

	/* We need to synchronize with __set_oom_adj */
	mutex_lock(&oom_adj_mutex);
	set_bit(MMF_MULTIPROCESS, &tsk->mm->flags);
	/* Update the values in case they were changed after copy_signal */
	tsk->signal->oom_score_adj = current->signal->oom_score_adj;
	tsk->signal->oom_score_adj_min = current->signal->oom_score_adj_min;
	mutex_unlock(&oom_adj_mutex);
}