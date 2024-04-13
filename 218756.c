static int memcg_charge_kernel_stack(struct task_struct *tsk)
{
#ifdef CONFIG_VMAP_STACK
	struct vm_struct *vm = task_stack_vm_area(tsk);
	int ret;

	BUILD_BUG_ON(IS_ENABLED(CONFIG_VMAP_STACK) && PAGE_SIZE % 1024 != 0);

	if (vm) {
		int i;

		BUG_ON(vm->nr_pages != THREAD_SIZE / PAGE_SIZE);

		for (i = 0; i < THREAD_SIZE / PAGE_SIZE; i++) {
			/*
			 * If memcg_kmem_charge_page() fails, page->mem_cgroup
			 * pointer is NULL, and memcg_kmem_uncharge_page() in
			 * free_thread_stack() will ignore this page.
			 */
			ret = memcg_kmem_charge_page(vm->pages[i], GFP_KERNEL,
						     0);
			if (ret)
				return ret;
		}
	}
#endif
	return 0;
}