static void do_up_read(struct irq_work *entry)
{
	struct stack_map_irq_work *work;

	if (WARN_ON_ONCE(IS_ENABLED(CONFIG_PREEMPT_RT)))
		return;

	work = container_of(entry, struct stack_map_irq_work, irq_work);
	mmap_read_unlock_non_owner(work->mm);
}