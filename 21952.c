static int __init stack_map_init(void)
{
	int cpu;
	struct stack_map_irq_work *work;

	for_each_possible_cpu(cpu) {
		work = per_cpu_ptr(&up_read_work, cpu);
		init_irq_work(&work->irq_work, do_up_read);
	}
	return 0;
}