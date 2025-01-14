static struct task_struct *dup_task_struct(struct task_struct *orig)
{
	struct task_struct *tsk;
	struct thread_info *ti;
	int err;

	prepare_to_copy(orig);

	tsk = alloc_task_struct();
	if (!tsk)
		return NULL;

	ti = alloc_thread_info(tsk);
	if (!ti) {
		free_task_struct(tsk);
		return NULL;
	}

 	err = arch_dup_task_struct(tsk, orig);
	if (err)
		goto out;

	tsk->stack = ti;

	err = prop_local_init_single(&tsk->dirties);
	if (err)
		goto out;

	setup_thread_stack(tsk, orig);

#ifdef CONFIG_CC_STACKPROTECTOR
	tsk->stack_canary = get_random_int();
#endif

	/* One for us, one for whoever does the "release_task()" (usually parent) */
	atomic_set(&tsk->usage,2);
	atomic_set(&tsk->fs_excl, 0);
#ifdef CONFIG_BLK_DEV_IO_TRACE
	tsk->btrace_seq = 0;
#endif
	tsk->splice_pipe = NULL;
	return tsk;

out:
	free_thread_info(ti);
	free_task_struct(tsk);
	return NULL;
}