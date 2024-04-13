static void account_kernel_stack(struct task_struct *tsk, int account)
{
	void *stack = task_stack_page(tsk);
	struct vm_struct *vm = task_stack_vm_area(tsk);


	/* All stack pages are in the same node. */
	if (vm)
		mod_lruvec_page_state(vm->pages[0], NR_KERNEL_STACK_KB,
				      account * (THREAD_SIZE / 1024));
	else
		mod_lruvec_slab_state(stack, NR_KERNEL_STACK_KB,
				      account * (THREAD_SIZE / 1024));
}