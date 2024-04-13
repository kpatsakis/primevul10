static unsigned long *alloc_thread_stack_node(struct task_struct *tsk,
						  int node)
{
	unsigned long *stack;
	stack = kmem_cache_alloc_node(thread_stack_cache, THREADINFO_GFP, node);
	stack = kasan_reset_tag(stack);
	tsk->stack = stack;
	return stack;
}